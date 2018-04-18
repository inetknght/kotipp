FILE(GLOB KOTI_NET_TEST_SRC "*.cpp")
FILE(GLOB KOTI_NET_TEST_HDR "*.hpp")

ADD_EXECUTABLE(koti_net_test ${KOTI_NET_TEST_SRC} ${KOTI_NET_TEST_HDR})

TARGET_LINK_LIBRARIES(koti_net_test
	koti_net_lib
	gtest_main
)

GTEST_ADD_TESTS(koti_net_test "" ${KOTI_NET_TEST_SRC})

