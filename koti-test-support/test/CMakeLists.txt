FILE(GLOB KOTI_TEST_SUPPORT_TEST_SRC "*.cpp")
FILE(GLOB KOTI_TEST_SUPPORT_TEST_HDR "*.hpp")

ADD_EXECUTABLE(koti_test_support_test ${KOTI_TEST_SUPPORT_TEST_SRC} ${KOTI_TEST_SUPPORT_TEST_HDR})

TARGET_LINK_LIBRARIES(koti_test_support_test
	koti_test_support_lib
	gtest_main
)

GTEST_ADD_TESTS(koti_test_support_test "" ${KOTI_TEST_SUPPORT_TEST_SRC})

