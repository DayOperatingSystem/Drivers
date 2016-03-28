set(HEADERS BinaryTree.h RAMFilesystem.h)
set(SOURCES BinaryTree.cpp RAMFilesystem.cpp)

add_executable(ramfs.drv ${SOURCES} ${HEADERS} main.cpp)
install(TARGETS ramfs.drv DESTINATION ${CMAKE_INSTALL_PREFIX}/image/system)

add_executable(ramfstest ${SOURCES} ${HEADERS} ${TESTSUITE_SRC} tests/main.cpp)

target_link_libraries(ramfs.drv dayos m stdc++ gcc DriverKit)
target_link_libraries(ramfstest dayos m stdc++ gcc DriverKit)

include_directories(${LIBDAYOS_INCLUDE} ${DRIVERKIT_INCLUDE} ${TESTSUITE_INCLUDE})
