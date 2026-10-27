# Registers AudIO's JUCE modules with the Kick build (Kick/3rdParty.cmake includes this file).
# The original was not in the release. AudIO_constants and AudIO_juce are reconstructions:
# see their headers.
juce_add_modules(
    ${CMAKE_CURRENT_LIST_DIR}/AudIO
    ${CMAKE_CURRENT_LIST_DIR}/AudIO_constants
    ${CMAKE_CURRENT_LIST_DIR}/AudIO_juce
)
