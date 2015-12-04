CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

ADD_SUBDIRECTORY(ug-setting-notification-efl)
ADD_SUBDIRECTORY(ug-setting-notification-do-not-disturb-efl)
ADD_SUBDIRECTORY(ug-setting-notification-app-notifications-efl)

ADD_DEPENDENCIES(ug-ug-setting-notification-do-not-disturb-efl ug-setting-notification-efl)
ADD_DEPENDENCIES(ug-ug-setting-notification-app-notifications-efl ug-setting-notification-efl)

MESSAGE("notification-setting main End")
