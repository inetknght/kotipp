CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

PROJECT(kotipp CXX)

# Be more explicit about link library search paths
CMAKE_POLICY(SET CMP0003 NEW)

# IN_LIST operator support
CMAKE_POLICY(SET CMP0057 NEW)

# If CMAKE_BUILD_TYPE is not configured, then set to default
# {None,Debug,RelWithDebInfo,Release,MinSizeRel}
IF (NOT CMAKE_BUILD_TYPE)
	SET (CMAKE_BUILD_TYPE "RelWithDebInfo")
ENDIF()
MESSAGE (STATUS "Current build type is ${CMAKE_BUILD_TYPE}")

# Must find threads. We don't specifically use threading in our model, but
# do use some supporting features of it
FIND_PACKAGE(Threads REQUIRED)

# Enable cmake test features
enable_testing()
INCLUDE(FindGTest)

# Enable GetGitRevisionDescription module
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
INCLUDE(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
git_describe(GIT_DESCRIBE --tags --long)
STRING(REGEX REPLACE "v([0-9]+)\\.([0-9]+)\\.([0-9]+)-([0-9a-z]+)" "\\1.\\2.\\3" VERSION "${GIT_DESCRIBE}")

MESSAGE(STATUS "Refspec: ${GIT_REFSPEC} Sha1: ${GIT_SHA1}")
MESSAGE(STATUS "Describe: ${GIT_DESCRIBE}")
MESSAGE(STATUS "Version: ${VERSION}")

SET(CPACK_PACKAGE_VERSION ${VERSION})
SET(CPACK_GENERATOR "RPM")
SET(CPACK_PACKAGE_NAME "kotipp")
SET(CPACK_PACKAGE_RELEASE 1)
SET(CPACK_PACKAGE_CONTACT "Keith Bennett <kotipp@knightoftheinter.net>")
SET(CPACK_PACKAGE_VENDOR "Knight of the Internet")
SET(CPACK_PACKAGE_RELOCATABLE ON)
SET(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
IF(CPack_CMake_INCLUDED)
ELSE()
	INCLUDE(CPack)
ENDIF()

# Find Boost for subprojects
SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)
FIND_PACKAGE(Boost 1.62.0 REQUIRED COMPONENTS program_options system filesystem iostreams)
INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS})

# Address Sanitizer - available in clang 3.2+ and gcc 4.8+
# Remember, you can turn on LEAK_SANTIZIER by setting environment variable ASAN_OPTIONS=detect_leaks=1 when running executable.
OPTION(ASAN_BUILD "When on, we will build for and link against Address Sanitizer" off)
IF(ASAN_BUILD)
        MESSAGE(STATUS "Address Sanitizer libraries selected")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
        SET(LINK_FLAGS "${LINK_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
ENDIF()

ADD_SUBDIRECTORY(thirdparty)
INCLUDE_DIRECTORIES(SYSTEM ${GTEST_INCLUDE_DIRS})

# -Werror=switch-enum
SET(COMMON_FLAGS "${COMMON_FLAGS} -Werror=all -Werror=unknown-pragmas -Werror=return-type -Wno-long-long -pedantic-errors -Wno-unused-function -Wno-vla -Werror=strict-aliasing")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 ${COMMON_FLAGS}")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 ${COMMON_FLAGS}")

# Find and set up zlib (anything using zlib needs to TARGET_LINK_LIBRARIES(target ${ZLIB_LIRARIES})
FIND_PACKAGE(ZLIB)
INCLUDE_DIRECTORIES(SYSTEM ${ZLIB_INCLUDE_DIRS})

# Find and set up bzip2 (anything using bzip2 needs to TARGET_LINK_LIBRARIES(target ${BZIP2_LIBRARIES})
FIND_PACKAGE(BZip2 REQUIRED)
INCLUDE_DIRECTORIES(SYSTEM ${BZIP2_INCLUDE_DIRS})

# kotipp: main support library and test executable
ADD_SUBDIRECTORY(kotipp)

ADD_SUBDIRECTORY(koti-test-support)
ADD_SUBDIRECTORY(koti-exceptions)
ADD_SUBDIRECTORY(koti-options)
ADD_SUBDIRECTORY(koti-application)

# koti-tcp: tcpip networking library.
# todo: refactor into koti-network, koti-tcp, koti-domain, etc
ADD_SUBDIRECTORY(koti-tcp)

INSTALL(EXPORT
	kotipp
	DESTINATION
	cmake-export/kotipp
)

EXPORT(PACKAGE kotipp)
