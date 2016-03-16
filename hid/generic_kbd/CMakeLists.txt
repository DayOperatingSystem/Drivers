file(GLOB_RECURSE HEADERS *.h)
file(GLOB_RECURSE SOURCES *.c*)

add_executable(kbd.drv ${SOURCES} ${HEADERS})
install(TARGETS kbd.drv DESTINATION ${CMAKE_INSTALL_PREFIX}/image/system)

target_link_libraries(kbd.drv dayos m stdc++ gcc DriverKit)
include_directories(${LIBDAYOS_INCLUDE} ${DRIVERKIT_INCLUDE})
