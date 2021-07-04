project(DistDemo)
include(CPack)

install(TARGETS trespass DefaultConfigGenerator RegToIniConverter DESTINATION ".")
install(DIRECTORY "${CMAKE_SOURCE_DIR}/Distribution/Binary/" DESTINATION ".")
install(DIRECTORY "${CMAKE_SOURCE_DIR}/Distribution/Demo/" DESTINATION ".")
