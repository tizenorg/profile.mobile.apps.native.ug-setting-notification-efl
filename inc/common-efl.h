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

#ifndef __COMMON_EFL_H__
#define __COMMON_EFL_H__

#include <Evas.h>
#include <stdbool.h>
#include <notification_setting.h>
#include <efl_extension.h>
#include <string.h>

#include <Elementary.h>
#include <package_manager.h>
#include "log.h"
#include "pkgmgr-setting-info.h"
#include "notification-setting-info.h"


#define PKGNAME NULL
#define APP_STRING(str)			dgettext(NULL, str)
#define MAX_TEXT_SIZE 1024

typedef struct _data_list data_list_t;
struct _data_list
{
	data_list_t *next;
	int id;
	int index;
	char *appid;
	char *name;
	char *icon;
	bool is_app_data;
};

typedef struct item_data
{
	int index;
	Elm_Object_Item *item;
	data_list_t *data;
} item_data_s;


/* User created ug data */
typedef struct ug_data_t
{
	Evas_Object *win;
	Evas_Object *layout;
	Evas_Object *naviframe;
	Evas_Object *list_main;
	Evas_Object *list_sub;
	Evas_Object *cancel_button;
	Evas_Object *done_button;
	Elm_Object_Item *navi_item;

} ug_data;


enum {
	ITEM_STYLE_DEFAULT = 0,
	ITEM_STYLE_TYPE_ONE,
	ITEM_STYLE_ONE_ICON,
	ITEM_STYLE_RADIO,
};


ug_data* get_app_ui_data() ;
void set_app_ui_data(ug_data *data);

Evas_Object *create_layout(Evas_Object *parent);
Evas_Object *create_background(Evas_Object *parent);
Evas_Object *create_naviframe(Evas_Object *parent);
Evas_Object* create_icon(Evas_Object *parent, char* icon_path);
Evas_Object* create_custom_layout(Evas_Object *parent, char* group_name);

void gl_loaded_cb(void *data EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED);
void gl_realized_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info);
void gl_longpressed_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info);

void gl_selected_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event_info);
void gl_contracted_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info);
void gl_del_cb(void *data, Evas_Object *obj EINA_UNUSED);

void append_gl_group_index(Evas_Object *genlist, char* text);
/**
 * @brief Append Eina_List to genlist and add radio button to each list item
 * @param genlist The genlist object
 * @param list List of selected items
 * @param style Name of the visual style to use for genlist. If you don't know use "default"
 */
void append_gl_radio_item_list(Evas_Object *genlist, Eina_List* list, char *style);
void append_gl_item_list(Evas_Object *genlist, Eina_List* list, int style);
void append_gl_start_option(Evas_Object *genlist, char *style, char *ugName);
Elm_Widget_Item *append_gl_allow_all(Evas_Object *genlist);
void append_gl_full_item(Evas_Object *genlist, Evas_Object *(*fullContentCb)(Evas_Object* parent, void *data), void *cbData);


void back_button_cb(void *data, Evas_Object *obj, void *event_info);
/**
 * @brief A callback function that canceled operation and returns to previous menu
 * @param data User data to be passed to the callback function
 * @param obj A smart object
 * @param event_info The event's info name string
 */
void cancel_button_noti_ls_cb(void *data, Evas_Object *obj, void *event_info);
/**
 * @brief A callback function that confirms operation
 * @param data User data to be passed to the callback function
 * @param obj A smart object
 * @param event_info The event's info name string
 */
void done_button_noti_ls_cb(void *data, Evas_Object *obj, void *event_info);


#endif //__COMMON_EFL_H__
