file(GLOB_RECURSE HEADERS *.h)
file(GLOB_RECURSE SOURCES *.c*)

add_executable(ansi_tty.drv ${SOURCES} ${HEADERS})
install(TARGETS ansi_tty.drv DESTINATION ${CMAKE_INSTALL_PREFIX}/image/system)
install(DIRECTORY keymaps DESTINATION ${CMAKE_INSTALL_PREFIX}/image/system)

target_link_libraries(ansi_tty.drv DriverKit dayos stdc++ gcc)
include_directories(${LIBDAYOS_INCLUDE} ${DRIVERKIT_INCLUDE})
