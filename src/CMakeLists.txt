add_definitions(-DCONFFILE=\"${CMAKE_INSTALL_PREFIX}etc/mausberry-switch.conf\")

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCONFIG REQUIRED libconfig>=1.4)
include_directories(${LIBCONFIG_INCLUDE_DIRS})
link_directories(${LIBCONFIG_LIBRARY_DIRS})
add_executable(mausberry-switch switch.c)
target_link_libraries(mausberry-switch ${LIBCONFIG_LIBRARIES})
install(TARGETS mausberry-switch DESTINATION usr/bin COMPONENT binaries)