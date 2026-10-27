#include "juce_serialport.h"

#if JUCE_LINUX || JUCE_MAC || JUCE_BSD
    #include <fcntl.h>
    #include <poll.h>
    #include <termios.h>
    #include <unistd.h>
    #define SERIALPORT_POSIX 1
#else
    #define SERIALPORT_POSIX 0
#endif

#if SERIALPORT_POSIX
static speed_t toSpeed( int bps )
{
    switch( bps )
    {
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
        default: return B9600;
    }
}
#endif

SerialPort::SerialPort( const juce::String& path, const SerialPortConfig& config, DebugFunction debug_ )
    : debugFunction( std::move( debug_ ) )
{
    if( open( path ) )
        setConfig( config );
}

SerialPort::~SerialPort() { close(); }

bool SerialPort::open( const juce::String& path )
{
    close();
    portPath = path;
#if SERIALPORT_POSIX
    fd = ::open( path.toRawUTF8(), O_RDWR | O_NOCTTY | O_NONBLOCK );
    if( fd < 0 )
    {
        debug( "SerialPort::open failed", path );
        return false;
    }
    return true;
#else
    debug( "SerialPort: no implementation on this platform", path );
    return false;
#endif
}

void SerialPort::close()
{
#if SERIALPORT_POSIX
    if( fd >= 0 )
        ::close( fd );
#endif
    fd = -1;
}

bool SerialPort::setConfig( const SerialPortConfig& config )
{
#if SERIALPORT_POSIX
    if( fd < 0 ) return false;

    termios tio{};
    if( tcgetattr( fd, &tio ) != 0 ) return false;
    cfmakeraw( &tio );
    cfsetispeed( &tio, toSpeed( config.bps ) );
    cfsetospeed( &tio, toSpeed( config.bps ) );

    tio.c_cflag &= ~(tcflag_t)CSIZE;
    tio.c_cflag |= config.databits == 7 ? CS7 : CS8;
    tio.c_cflag |= CREAD | CLOCAL;

    tio.c_cflag &= ~(tcflag_t)( PARENB | PARODD );
    if( config.parity == SerialPortConfig::SERIALPORT_PARITY_EVEN ) tio.c_cflag |= PARENB;
    if( config.parity == SerialPortConfig::SERIALPORT_PARITY_ODD ) tio.c_cflag |= PARENB | PARODD;

    if( config.stopbits == SerialPortConfig::STOPBITS_2 ) tio.c_cflag |= CSTOPB;
    else tio.c_cflag &= ~(tcflag_t)CSTOPB;

    tio.c_cflag &= ~(tcflag_t)CRTSCTS;
    tio.c_iflag &= ~(tcflag_t)( IXON | IXOFF );
    if( config.flowcontrol == SerialPortConfig::FLOWCONTROL_HARDWARE ) tio.c_cflag |= CRTSCTS;
    if( config.flowcontrol == SerialPortConfig::FLOWCONTROL_XONXOFF ) tio.c_iflag |= IXON | IXOFF;

    tio.c_cc[VMIN]  = 0;
    tio.c_cc[VTIME] = 0;
    return tcsetattr( fd, TCSANOW, &tio ) == 0;
#else
    juce::ignoreUnused( config );
    return false;
#endif
}

juce::StringPairArray SerialPort::getSerialPortPaths()
{
    juce::StringPairArray ports;
#if SERIALPORT_POSIX
    juce::File dev( "/dev" );
    #if JUCE_MAC
    const juce::String pattern = "cu.*";
    #else
    const juce::String pattern = "tty*";
    #endif
    for( const auto& f : dev.findChildFiles( juce::File::findFiles, false, pattern ) )
    {
        const auto name = f.getFileName();
        #if JUCE_LINUX || JUCE_BSD
        if( !( name.startsWith( "ttymxc" ) || name.startsWith( "ttyUSB" ) || name.startsWith( "ttyACM" ) || name.startsWith( "ttyS" ) ) )
            continue;
        #endif
        ports.set( name, f.getFullPathName() );
    }
#endif
    return ports;
}

SerialPortInputStream::SerialPortInputStream( SerialPort* port_ )
    : juce::Thread( "SerialPortInputStream" ), port( port_ )
{
    startThread();
}

SerialPortInputStream::~SerialPortInputStream() { stopThread( 1000 ); }

void SerialPortInputStream::setNotify( NotifyMode mode, char c )
{
    notifyChar = c;
    notify     = mode;
}

juce::int64 SerialPortInputStream::getTotalLength()
{
    const juce::ScopedLock sl( bufferLock );
    return (juce::int64)bufferedBytes;
}

bool SerialPortInputStream::isExhausted() { return getTotalLength() == 0; }

int SerialPortInputStream::read( void* destBuffer, int maxBytesToRead )
{
    const juce::ScopedLock sl( bufferLock );
    const auto n = (size_t)juce::jmin( (juce::int64)maxBytesToRead, (juce::int64)bufferedBytes );
    if( n == 0 ) return 0;
    std::memcpy( destBuffer, buffer.getData(), n );
    std::memmove( buffer.getData(), static_cast<char*>( buffer.getData() ) + n, bufferedBytes - n );
    bufferedBytes -= n;
    return (int)n;
}

void SerialPortInputStream::run()
{
#if SERIALPORT_POSIX
    char chunk[512];
    while( !threadShouldExit() )
    {
        if( port == nullptr || !port->exists() )
        {
            wait( 100 );
            continue;
        }
        pollfd pfd{ port->getFileDescriptor(), POLLIN, 0 };
        if( ::poll( &pfd, 1, 100 ) <= 0 ) continue;

        const auto got = ::read( port->getFileDescriptor(), chunk, sizeof( chunk ) );
        if( got <= 0 ) continue;

        bool sawChar = false;
        {
            const juce::ScopedLock sl( bufferLock );
            if( buffer.getSize() < bufferedBytes + (size_t)got )
                buffer.setSize( ( bufferedBytes + (size_t)got ) * 2 );
            std::memcpy( static_cast<char*>( buffer.getData() ) + bufferedBytes, chunk, (size_t)got );
            bufferedBytes += (size_t)got;
            sawChar = std::memchr( chunk, notifyChar, (size_t)got ) != nullptr;
        }
        if( notify == NOTIFY_ALWAYS || ( notify == NOTIFY_ON_CHAR && sawChar ) )
            sendChangeMessage();
    }
#endif
}

void SerialPortOutputStream::flush()
{
#if SERIALPORT_POSIX
    if( port != nullptr && port->exists() ) tcdrain( port->getFileDescriptor() );
#endif
}

bool SerialPortOutputStream::write( const void* data, size_t numBytes )
{
#if SERIALPORT_POSIX
    if( port == nullptr || !port->exists() ) return false;
    auto*  p    = static_cast<const char*>( data );
    size_t left = numBytes;
    while( left > 0 )
    {
        const auto n = ::write( port->getFileDescriptor(), p, left );
        if( n < 0 )
        {
            if( errno == EAGAIN ) { juce::Thread::sleep( 1 ); continue; }
            return false;
        }
        p += n;
        left -= (size_t)n;
    }
    written += (juce::int64)numBytes;
    return true;
#else
    juce::ignoreUnused( data, numBytes );
    return false;
#endif
}
