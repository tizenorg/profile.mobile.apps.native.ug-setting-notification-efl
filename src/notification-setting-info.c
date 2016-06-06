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


#include "notification-setting-info.h"
#include <notification.h>
#include <notification_setting.h>
#include <notification_setting_internal.h>

setting_info_s *setting_info = NULL;

#define VOICE_CALL_PACKAGE "org.tizen.call-ui"

void create_app_notification_list()
{
    NOTISET_TRACE_BEGIN;
	int i = 0;
	int count = 0;
	char *package_name = NULL;
	bool allow_to_notify = false;
	bool do_not_disturb_except = false;
	notification_setting_h setting_array = NULL;
	notification_setting_h temp = NULL;
	item_info_s *item_info = NULL;

	remove_all_apps_list();
	setting_info = calloc(1, sizeof(setting_info_s));

	notification_setting_get_setting_array(&setting_array, &count);
	ret_if(!setting_array);
	NOTISET_DBG("count %d", count);

	for (i = 0; i < count; i++) {
		item_info = calloc(1, sizeof(item_info_s));
		if (item_info) {
			temp = setting_array + i;

			notification_setting_get_package_name(temp, &package_name);
			item_info->appid = package_name;

			notification_setting_get_allow_to_notify(temp, &allow_to_notify);
			item_info->allow_to_notify = allow_to_notify;

			notification_setting_get_do_not_disturb_except(temp, &do_not_disturb_except);
			item_info->do_not_disturb_except = do_not_disturb_except;

			item_info->icon = get_app_icon(package_name);
			item_info->name = get_app_name(package_name);

			item_info->index = i;

			if (item_info->name && strcmp(package_name, VOICE_CALL_PACKAGE) != 0) {
				setting_info->all_apps_list = eina_list_append(setting_info->all_apps_list, item_info);
			} else {
				FREEIF(package_name);
				FREEIF(item_info->name);
				FREEIF(item_info->icon);
				FREEIF(item_info);
			}
		}
	}

	setting_info->all_apps_list = eina_list_sort(setting_info->all_apps_list, eina_list_count(setting_info->all_apps_list), apps_sort_cb);
	notification_setting_free_notification(setting_array);
}

void create_do_not_disturb_application_list()
{
	NOTISET_DBG("create_do_not_disturb_application_list");
	int i = 0;
	int count = 0;
	char *package_name = NULL;
	bool allow_to_notify = false;
	bool do_not_disturb_except = false;
	notification_setting_h setting_array = NULL;
	notification_setting_h temp = NULL;

	remove_excepted_apps_list();
	setting_info = calloc(1, sizeof(setting_info_s));

	notification_setting_get_setting_array(&setting_array, &count);
	ret_if(!setting_array);
	NOTISET_DBG("count %d", count);

	item_info_s *item_info = NULL;
	for (i = 0; i < count; i++) {
		item_info = calloc(1, sizeof(item_info_s));
		if (item_info) {
			temp = setting_array + i;

			notification_setting_get_package_name(temp, &package_name);
			item_info->appid = package_name;

			notification_setting_get_allow_to_notify(temp, &allow_to_notify);
			item_info->allow_to_notify = allow_to_notify;

			notification_setting_get_do_not_disturb_except(temp, &do_not_disturb_except);
			item_info->do_not_disturb_except = do_not_disturb_except;

			item_info->icon = get_app_icon(package_name);
			item_info->name = get_app_name(package_name);

			item_info->index = i;

			if(item_info->name) {
				if (do_not_disturb_except) {
					setting_info->excepted_list = eina_list_append(setting_info->excepted_list, item_info);
				} else {
					setting_info->not_excepted_list = eina_list_append(setting_info->not_excepted_list, item_info);
				}
			} else {
				FREEIF(package_name);
				FREEIF(item_info->name);
				FREEIF(item_info->icon);
				FREEIF(item_info);
			}
		}
	}

	setting_info->excepted_list = eina_list_sort(setting_info->excepted_list, eina_list_count(setting_info->excepted_list), apps_sort_cb);
	setting_info->not_excepted_list = eina_list_sort(setting_info->not_excepted_list, eina_list_count(setting_info->not_excepted_list), apps_sort_cb);

	notification_setting_free_notification(setting_array);
}

Eina_List *get_excepted_apps_list()
{
	NOTISET_TRACE_BEGIN;
	return setting_info->excepted_list;
}

Eina_List *get_all_apps_list()
{
	NOTISET_TRACE_BEGIN;
	return setting_info->all_apps_list;
}

bool get_allow_all()
{
	item_info_s *item = NULL;
	Eina_List *list = get_all_apps_list();

	int count = eina_list_count(list);
	NOTISET_DBG("count %d", count);

	if (count == 0)
		return false;

	while (list) {
		item = (item_info_s*)eina_list_data_get(list);
		if(item && !item->allow_to_notify)
			return false;
		list = eina_list_next(list);
	}

	return true;
}

void set_allow_all(bool state)
{
	NOTISET_TRACE_BEGIN;

	item_info_s *item = NULL;
	Eina_List *list = get_all_apps_list();

	int count = eina_list_count(list);
	NOTISET_DBG("count %d", count);

	while (list) {
		item = (item_info_s*)eina_list_data_get(list);
		if(item) {
			bool res = set_allow_to_nofity(item->appid, state);
			if (res)
				item->allow_to_notify = state;
		}
		list = eina_list_next(list);
	}
}

Eina_List *get_not_excepted_apps_list()
{
	NOTISET_TRACE_BEGIN;
	return setting_info->not_excepted_list;
}


bool get_do_not_disturb() 
{
	NOTISET_TRACE_BEGIN;

	int err = NOTIFICATION_ERROR_NONE;
	notification_system_setting_h system_setting = NULL;
	bool do_not_disturb = false;

	err = notification_system_setting_load_system_setting(&system_setting);
	if (err != NOTIFICATION_ERROR_NONE || system_setting == NULL) {
		NOTISET_ERR("notification_system_setting_load_system_setting failed [%d]\n", err);
		goto out;
	}

	notification_system_setting_get_do_not_disturb(system_setting, &do_not_disturb);
	NOTISET_DBG("do_not_disturb [%d]\n", do_not_disturb);
	
out:
	if (system_setting)
		notification_system_setting_free_system_setting(system_setting);

	return do_not_disturb;
}

void set_do_not_disturb(bool state) 
{
	NOTISET_TRACE_BEGIN;
	int err = NOTIFICATION_ERROR_NONE;
	notification_system_setting_h system_setting = NULL;

	err = notification_system_setting_load_system_setting(&system_setting);
	if (err != NOTIFICATION_ERROR_NONE || system_setting == NULL) {
		NOTISET_ERR("notification_system_setting_load_system_setting failed [%d]\n", err);
		goto out;
	}

	notification_system_setting_set_do_not_disturb(system_setting, state);
	NOTISET_DBG("set do_not_disturb [%d]\n", state);

	err = notification_system_setting_update_system_setting(system_setting);
	if (err != NOTIFICATION_ERROR_NONE) {
		NOTISET_ERR("notification_setting_update_setting [%d]\n", err);
		goto out;
	}
out:
	if (system_setting)
		notification_system_setting_free_system_setting(system_setting);

}

bool set_allow_to_nofity(char *pkg_name, bool state)
{
	NOTISET_TRACE_BEGIN;
	int err = NOTIFICATION_ERROR_NONE;

	notification_setting_h setting = NULL;

	err = notification_setting_get_setting_by_package_name(pkg_name, &setting);
	if (err != NOTIFICATION_ERROR_NONE || setting == NULL) {
		NOTISET_ERR("notification_setting_get_setting_by_package_name [%d]\n", err);
		goto out;
	}

	notification_setting_set_allow_to_notify(setting, state);
	NOTISET_DBG("notification_setting_set_allow_to_notify [%d]\n", state);

	err = notification_setting_update_setting(setting);
	if (err != NOTIFICATION_ERROR_NONE) {
		NOTISET_ERR("notification_setting_update_setting [%d]\n", err);
		goto out;
	}

	if (setting)
		notification_setting_free_notification(setting);
	return true;

out:
	if (setting)
		notification_setting_free_notification(setting);
	return false;

}

bool set_excepted_apps(char *pkg_name, bool state)
{
	NOTISET_TRACE_BEGIN;

	int err = NOTIFICATION_ERROR_NONE;

	notification_setting_h setting = NULL;

	err = notification_setting_get_setting_by_package_name(pkg_name, &setting);
	if (err != NOTIFICATION_ERROR_NONE || setting == NULL) {
		NOTISET_ERR("notification_setting_get_setting_by_package_name [%d]\n", err);
		goto out;
	}

	notification_setting_set_do_not_disturb_except(setting, state);
	NOTISET_DBG("notification_setting_set_do_not_disturb_except [%s] [%d]\n", pkg_name, state);

	err = notification_setting_update_setting(setting);
	if (err != NOTIFICATION_ERROR_NONE) {
		NOTISET_ERR("notification_setting_update_setting err[%d]\n", err);
	}

	if (setting)
		notification_setting_free_notification(setting);
	return true;

out:
	if (setting)
		notification_setting_free_notification(setting);

	return false;

}

static void _remove_apps_list(Eina_List* input_list) {
	NOTISET_TRACE_BEGIN;
	item_info_s *item_info = NULL;

	if(input_list) {
		EINA_LIST_FREE(input_list, item_info) {
			//NOTISET_DBG("remove %s", item_info->name);
			FREEIF(item_info->appid);
			FREEIF(item_info->name);
			FREEIF(item_info->icon);
			FREEIF(item_info);
		}
	}
}


void remove_all_apps_list() {
	NOTISET_TRACE_BEGIN;
	if(setting_info) {
		_remove_apps_list(setting_info->all_apps_list);
	}
}
void remove_excepted_apps_list(){
	NOTISET_TRACE_BEGIN;
	if(setting_info) {
		_remove_apps_list(setting_info->excepted_list);
		_remove_apps_list(setting_info->not_excepted_list);
	}

}

