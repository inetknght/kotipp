FILE(GLOB KOTI_EXCEPTIONS_TEST_SRC "*.cpp")
FILE(GLOB KOTI_EXCEPTIONS_TEST_HDR "*.hpp")

ADD_EXECUTABLE(koti_exceptions_test ${KOTI_EXCEPTIONS_TEST_SRC} ${KOTI_EXCEPTIONS_TEST_HDR})

TARGET_LINK_LIBRARIES(koti_exceptions_test
	koti_exceptions_lib
	gtest_main
)

GTEST_ADD_TESTS(koti_exceptions_test "" ${KOTI_EXCEPTIONS_TEST_SRC})

