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
#include <ui-gadget-module.h>
#include <efl_extension.h>

#include <Elementary.h>
#include <package-manager.h>
#include <pkgmgr-info.h>
#include "log.h"
#include "pkgmgr-setting-info.h"
#include "notification-setting-info.h"


#define PKGNAME "ug-setting-notification-efl"
#define APP_STRING(str)			dgettext(PKGNAME, str)


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
	ui_gadget_h ug;
	Evas_Object *layout;
	Evas_Object *naviframe;
	Evas_Object *list_main;
	Evas_Object *list_sub;
	Evas_Object *cancel_button;
	Evas_Object *done_button;

	int mode;

	Elm_Object_Item *navi_item;

} ug_data;


enum {
	ITEM_STYLE_DEFAULT = 0,
	ITEM_STYLE_ONE_LINE,
	ITEM_STYLE_ONE_ICON,
};


Evas_Object *create_layout(Evas_Object *parent);
Evas_Object *create_background(Evas_Object *parent);
Evas_Object *create_naviframe(Evas_Object *parent);

void gl_loaded_cb(void *data EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED);
void gl_realized_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info);
void gl_longpressed_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info);

void gl_selected_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event_info);
void gl_contracted_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info);
void gl_del_cb(void *data, Evas_Object *obj EINA_UNUSED);

void append_gl_group_index(Evas_Object *genlist, char* text);
void append_gl_item_list(Evas_Object *genlist, Eina_List* list, int style);
void append_gl_start_option(Evas_Object *genlist, char *style, char *ugName);


void back_button_cb(void *data, Evas_Object *obj, void *event_info);


#endif //__COMMON_EFL_H__
