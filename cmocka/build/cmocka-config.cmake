
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was cmocka-config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../" ABSOLUTE)

# Use original install prefix when loaded through a "/usr move"
# cross-prefix symbolic link such as /lib -> /usr/lib.
get_filename_component(_realCurr "${CMAKE_CURRENT_LIST_DIR}" REALPATH)
get_filename_component(_realOrig "/usr/lib/x86_64-linux-gnu/cmake/cmocka" REALPATH)
if(_realCurr STREQUAL _realOrig)
  set(PACKAGE_PREFIX_DIR "/usr")
endif()
unset(_realOrig)
unset(_realCurr)

####################################################################################

get_filename_component(cmocka_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
if(NOT TARGET cmocka::cmocka)
	include("${cmocka_CMAKE_DIR}/cmocka-targets.cmake")
endif()

# for backwards compatibility
set(CMOCKA_LIBRARY cmocka::cmocka)
set(CMOCKA_LIBRARIES cmocka::cmocka)
