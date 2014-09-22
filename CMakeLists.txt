cmake_minimum_required(VERSION 2.8.4)
project(mausberry_switch)
subdirs(src)
install(FILES etc/mausberry-switch.conf DESTINATION etc/ COMPONENT config)