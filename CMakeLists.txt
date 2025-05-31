cmake_minimum_required(VERSION 3.10)
project(hw4 VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

add_executable(main main.cpp)



include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "hw4")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "Your Name <your.email@example.com>")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "My Application")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "") 
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_GENERATOR "DEB") 
include(CPack)

