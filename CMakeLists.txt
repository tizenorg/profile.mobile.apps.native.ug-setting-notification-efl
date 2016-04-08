CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(setting-notification C)

# subdir
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(res/po)

CONFIGURE_FILE(setting-notification.manifest-3.0.in ${PKGNAME}.manifest)

INSTALL(FILES setting-notification.manifest.xml DESTINATION ${MANIFESTDIR} RENAME ${PKGNAME}.xml)
INSTALL(FILES shared/res/setting-notification.png DESTINATION ${ICONDIR})
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/res/images/ DESTINATION ${IMGDIR})
