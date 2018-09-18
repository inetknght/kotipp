
####################################
# Bring spdlog into the project.

IF(NOT TARGET spdlog)

ADD_LIBRARY(spdlog INTERFACE)
TARGET_INCLUDE_DIRECTORIES(spdlog
	INTERFACE 
	spdlog/include
)
TARGET_LINK_LIBRARIES(spdlog
	INTERFACE
	${CMAKE_THREAD_LIBS_INIT}
)

ENDIF()

# Bring spdlog into the project.
####################################

