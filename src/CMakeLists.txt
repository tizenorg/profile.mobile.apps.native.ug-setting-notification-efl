CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(setting-notification C)

SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(LIBDIR "${PREFIX}/lib")

SET(VERSION 0.1.0)

SET(SRCS
    allowed-calls.c
	common-efl.c
	do-not-disturb-efl.c
	excepted-apps-efl.c
	main.c
	notification-setting-info.c
    pkgmgr-setting-info.c
    set-schedule-info.c
)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/inc/)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED 
	elementary
	efl-extension
	edje
	evas
	ecore
	dlog
	notification
	pkgmgr
	pkgmgr-info
	capi-appfw-package-manager
	capi-system-system-settings
	capi-appfw-app-manager
	capi-appfw-application
)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CXXFLAGS "${EXTRA_CXXFLAGS} ${flag}")
ENDFOREACH(flag)

SET(EXTRA_CXXFLAGS "${EXTRA_CXXFLAGS} -fvisibility=hidden -Wall -Werror")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXXFLAGS}")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS}")

#Set executable
ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS})

# install binary
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${BINDIR})
