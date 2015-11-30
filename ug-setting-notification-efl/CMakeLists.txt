CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(ug-setting-notification-efl C)

SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(PROJECT_NAME "${PROJECT_NAME}")
SET(LIBDIR "${PREFIX}/apps/${PROJECT_NAME}/lib/ug")
SET(RESDIR "${PREFIX}/apps/${PROJECT_NAME}/res")
SET(DATADIR "${PREFIX}/apps/${PROJECT_NAME}/data")
SET(LOCALEDIR "${RESDIR}/locale")
SET(IMAGEDIR "${RESDIR}/images")
SET(EDJEDIR "${RESDIR}/edje")
SET(ETCDIR "${RESDIR}/etc")

SET(VERSION_MAJOR 0)
SET(VERSION "${VERSION_MAJOR}.0.1")

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED
	elementary
	efl-extension
	edje
	evas
	ecore
	ui-gadget-1
	dlog
	notification
	pkgmgr
	pkgmgr-info
	capi-appfw-package-manager
	capi-system-system-settings
	capi-appfw-app-manager
	capi-appfw-application
)

SET(BUILD_SOURCE
	src/common-efl.c
	src/pkgmgr-setting-info.c
	src/notification-setting-info.c
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/inc)
ADD_DEFINITIONS(${pkgs_CFLAGS} "-fPIC")
ADD_DEFINITIONS("-DPKGNAME=\"${PROJECT_NAME}\"")
#ADD_DEFINITIONS("-DLOCALE=\"${LOCALEDIR}\"")
ADD_DEFINITIONS("-DIMAGEDIR=\"${IMAGEDIR}\"")

ADD_LIBRARY(${PROJECT_NAME} STATIC ${BUILD_SOURCE})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS})
#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${bin_pkgs_LDFLAGS})
#SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SOVERSION ${VERSION_MAJOR})
#SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES VERSION ${VERSION})

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE DESTINATION /usr/share/license RENAME "${PROJECT_NAME}")
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.xml DESTINATION /usr/share/packages)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/${PROJECT_NAME}.png DESTINATION /usr/share/icons/default/small/)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION /usr/apps/ug-setting-notification-efl/lib/ug)

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/res/images/ DESTINATION ${IMAGEDIR}) 


MESSAGE("ug-setting-notification-efl End")

ADD_SUBDIRECTORY(res/po)

# End of a file
