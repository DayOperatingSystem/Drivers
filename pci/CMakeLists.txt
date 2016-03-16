file(GLOB_RECURSE HEADERS *.h)
file(GLOB_RECURSE SOURCES *.c*)

add_executable(pci.drv ${SOURCES} ${HEADERS})
install(TARGETS pci.drv DESTINATION ${CMAKE_INSTALL_PREFIX}/image/system)

target_link_libraries(pci.drv dayos m stdc++ gcc DriverKit)
include_directories(${LIBDAYOS_INCLUDE} ${DRIVERKIT_INCLUDE})
