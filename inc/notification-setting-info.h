/*
 * Copyright (c) 2009 - 2015 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifndef __NOTIFICATION_SETTING_INFO_H__
#define __NOTIFICATION_SETTING_INFO_H__

#include <Elementary.h>
#include "pkgmgr-setting-info.h"


typedef struct _setting_info_t {
    Eina_List *not_excepted_list;
    Eina_List *excepted_list;
    Eina_List *all_apps_list;
    Eina_List *first_allowed_list;
    Eina_List *first_excepted_list;
    Eina_List *show_all_content_list;
    Eina_List *hide_sensitive_content_list;
    Eina_List *lower_noti_list;
} setting_info_s;

bool get_do_not_disturb();
void set_do_not_disturb(bool state);

bool set_allow_to_nofity(char *pkg_name, bool state);
bool set_excepted_apps(char *pkg_name, bool state);

bool get_allow_all();
void set_allow_all(bool state);
Eina_List *get_all_apps_list();
Eina_List *get_first_allowed_apps_list();
Eina_List *get_first_excepted_apps_list();
Eina_List *get_excepted_apps_list();
Eina_List *get_not_excepted_apps_list();
/**
 *@brief Get list that should be shown in lower part of screen in lock screen content menu
 *@return Eina_List with commands that can be done in lock screen content menu
 */
Eina_List *get_lower_noti_list();

void create_app_notification_list();
void create_do_not_disturb_application_list();
/**
 *@brief Create list that should be shown in lower part of screen in lock screen content menu
 */
void create_notification_on_ls_list();
/**
 *@brief Remove list that should be shown in lower part of screen in lock screen content menu
 */
void remove_lower_noti_list();
void remove_all_apps_list();
void remove_excepted_apps_list();
void remove_arranged_list();


#endif /* __NOTIFICATION_SETTING_INFO_H__ */
