
FILE(GLOB KOTI_EXCEPTIONS_SRC "*.cpp" "exceptions/*.cpp")
FILE(GLOB KOTI_EXCEPTIONS_HDR "*.hpp" "exceptions/*.hpp")

SET(KOTI_EXCEPTIONS_LIB_DEPENDENCIES
	spdlog
	${Boost_LIBRARIES}
)

IF(KOTI_EXCEPTIONS_SRC)
	SET(KOTI_EXCEPTIONS_LIB_TYPE "STATIC")

	ADD_LIBRARY(koti_exceptions_lib STATIC
		${KOTI_EXCEPTIONS_HDR}
		${KOTI_EXCEPTIONS_SRC}
	)
	TARGET_LINK_LIBRARIES(koti_exceptions_lib
		spdlog
		${KOTI_EXCEPTIONS_LIB_DEPENDENCIES}
	)
ELSE()
	SET(KOTI_EXCEPTIONS_LIB_TYPE "INTERFACE")

	ADD_LIBRARY(koti_exceptions_lib INTERFACE)
	TARGET_LINK_LIBRARIES(koti_exceptions_lib
		INTERFACE
		spdlog
		${KOTI_EXCEPTIONS_LIB_DEPENDENCIES}
	)
ENDIF()

TARGET_INCLUDE_DIRECTORIES(koti_exceptions_lib
	INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}
)

INSTALL(TARGETS koti_exceptions_lib DESTINATION lib)

ADD_SUBDIRECTORY(test)

