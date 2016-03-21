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


#include <Elementary.h>
#include <libintl.h>
#include <package_manager.h>
#include "log.h"
#include "notification-setting-info.h"
#include "common-efl.h"
#include "do-not-disturb-efl.h"

static Evas_Object* _create_edit_exception_apps_list(ug_data *ugd)
{
	Evas_Object *parent = ugd->naviframe;
	Evas_Object *genlist;

	genlist = elm_genlist_add(parent);
	elm_genlist_mode_set(genlist, ELM_LIST_SCROLL);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);

	/* Add Smart Callback */
	evas_object_smart_callback_add(genlist, "realized", gl_realized_cb, NULL);
	evas_object_smart_callback_add(genlist, "loaded", gl_loaded_cb, NULL);
	evas_object_smart_callback_add(genlist, "longpressed", gl_longpressed_cb, NULL);
	evas_object_smart_callback_add(genlist, "contracted", gl_contracted_cb, NULL);

	append_gl_group_index(genlist, APP_STRING("IDS_ST_HEADER_EXCLUDED_APPS_ABB"));
	append_gl_item_list(genlist, get_excepted_apps_list(), ITEM_STYLE_DEFAULT);
	append_gl_group_index(genlist, APP_STRING("IDS_ST_HEADER_INCLUDED_APPS_ABB"));
	append_gl_item_list(genlist, get_not_excepted_apps_list(), ITEM_STYLE_DEFAULT);

	return genlist;
}


static void _done_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	NOTISET_DBG("");

	ug_data* u_data = data;
	Eina_List *list = NULL;

	elm_naviframe_item_pop(u_data->naviframe);

	EVAS_OBJECT_DELIF(u_data->done_button);
	EVAS_OBJECT_DELIF(u_data->cancel_button);
	EVAS_OBJECT_DELIF(u_data->list_sub);

	if (u_data->list_main) {
		list = get_excepted_apps_list();
		while (list) {
			item_info_s *item = (item_info_s*)eina_list_data_get(list);
			if (item->do_not_disturb_except == false) {
				set_excepted_apps(item->appid, item->do_not_disturb_except);
			}
			list = eina_list_next(list);
		}

		list = get_not_excepted_apps_list();
		while (list) {
			item_info_s *item = (item_info_s*)eina_list_data_get(list);
			if (item->do_not_disturb_except) {
				set_excepted_apps(item->appid, item->do_not_disturb_except);
			}
			list = eina_list_next(list);
		}

		create_do_not_disturb_application_list();
		elm_genlist_clear(u_data->list_main);
		do_not_disturb_append_item_in_list(u_data->list_main);

	}
}


void _cancel_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	ug_data* u_data = data;
	ret_if(!data);
	elm_naviframe_item_pop(u_data->naviframe);

	EVAS_OBJECT_DELIF(u_data->done_button);
	EVAS_OBJECT_DELIF(u_data->cancel_button);
	EVAS_OBJECT_DELIF(u_data->list_sub);
}

void exception_application_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	NOTISET_DBG("");
	ug_data* u_data = data;
	ret_if(!u_data);

	Elm_Object_Item* navi_item = NULL;
	Evas_Object *cancel_btn = NULL;
	Evas_Object *done_btn = NULL;

	create_do_not_disturb_application_list();

	u_data->list_sub = _create_edit_exception_apps_list(u_data);
	//genlist = NULL;

	/* Push to naviframe */
	navi_item = elm_naviframe_item_push(u_data->naviframe, APP_STRING("IDS_ST_HEADER_MANAGE_EXCLUDED_APPS_ABB"), NULL, NULL, u_data->list_sub, NULL);

	/* Title Cancel Button */
	cancel_btn = elm_button_add(u_data->naviframe);
	elm_object_style_set(cancel_btn, "naviframe/title_left");
	evas_object_smart_callback_add(cancel_btn, "clicked", _cancel_button_cb, u_data);
	elm_object_text_set(cancel_btn, APP_STRING("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
	elm_object_item_part_content_set(navi_item, "title_left_btn", cancel_btn);
	u_data->cancel_button = cancel_btn;

	/* Title Done Button */
	done_btn = elm_button_add(u_data->naviframe);
	elm_object_style_set(done_btn, "naviframe/title_right");
	evas_object_smart_callback_add(done_btn, "clicked", _done_button_cb, u_data);
	elm_object_text_set(done_btn, APP_STRING("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));
	elm_object_item_part_content_set(navi_item, "title_right_btn", done_btn);
	u_data->done_button = done_btn;

}

