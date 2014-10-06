cmake_minimum_required( VERSION 2.8 )
project( eva_cpp )
set( VERSION_NUMBER "0.0.1" )

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" )
    set( VERSION "${VERSION_NUMBER}-release" )
else()
    set( VERSION "${VERSION_NUMBER}-debug" )
endif()

find_package( Boost REQUIRED COMPONENTS thread system )

include_directories( include ${Boost_INCLUDE_DIRS} )
add_subdirectory( src )

install( FILES include/eva/event.hpp DESTINATION include/eva )

set( CPACK_PACKAGE_VERSION ${VERSION} )
set( CPACK_GENERATOR "RPM" )
set( CPACK_PACKAGE_NAME "eva_cpp" )
set( CPACK_PACKAGING_INSTALL_PREFIX "/usr/local/eva_cpp/${VERSION}" )
set( CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}.${CMAKE_SYSTEM_PROCESSOR}" )

include( CPack )
