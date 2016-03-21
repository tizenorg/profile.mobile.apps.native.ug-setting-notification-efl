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
#include <system_settings.h>
#include <app.h>
#include <app_control.h>

#include "notification-setting-info.h"
#include "common-efl.h"

/**
 * @{
 */

static Evas_Object *_create_win()
{
	/* Window: */
    Evas_Object *win = elm_win_util_standard_add("", "");
    elm_win_conformant_set(win, EINA_TRUE);
    elm_win_autodel_set(win, EINA_TRUE);
    evas_object_show(win);

    /*  Conform: */
    Evas_Object *conform = elm_conformant_add(win);
    elm_win_indicator_mode_set(conform, ELM_WIN_INDICATOR_SHOW);
    elm_win_indicator_opacity_set(conform, ELM_WIN_INDICATOR_OPAQUE);
    evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(win, conform);
    evas_object_show(conform);

    /*  Bg: */
    Evas_Object *bg = elm_bg_add(conform);
    evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_part_content_set(conform, "elm.swallow.bg", bg);
    evas_object_show(bg);

    return conform;
}

static Eina_Bool _notifiacation_setting_main_pop_cb(void *data, Elm_Object_Item * it)
{
	NOTISET_DBG("");
	ug_data *ugd = (ug_data *) data;
	retv_if(!ugd, EINA_FALSE);

	ui_app_exit();

	return EINA_FALSE;
}

Evas_Object* _create_app_notification_gl(ug_data *ugd)
{
	Evas_Object *parent = ugd->naviframe;
	Evas_Object *genlist;
	
	genlist = elm_genlist_add(parent);
	elm_genlist_mode_set(genlist, ELM_LIST_SCROLL);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_genlist_block_count_set(genlist, 8);

	/* Add Smart Callback */
	evas_object_smart_callback_add(genlist, "realized", gl_realized_cb, NULL);
	evas_object_smart_callback_add(genlist, "loaded", gl_loaded_cb, NULL);
	evas_object_smart_callback_add(genlist, "longpressed", gl_longpressed_cb, NULL);
	evas_object_smart_callback_add(genlist, "contracted", gl_contracted_cb, NULL);

	append_gl_start_option(genlist, "multiline", "app-notifications");
	append_gl_item_list(genlist, get_all_apps_list(), ITEM_STYLE_ONE_LINE);

	return genlist;
}

static void _create_main_view(void *data)
{
	NOTISET_DBG("create main view");
	ug_data *ugd = (ug_data *) data;
	ret_if(!ugd);

	/* Create genlist */
	ugd->list_main = _create_app_notification_gl(ugd);

	/* back Button */
	Evas_Object *back_btn = elm_button_add(ugd->naviframe);
	elm_object_style_set(back_btn, "naviframe/back_btn/default");
	evas_object_smart_callback_add(back_btn, "clicked", back_button_cb, ugd->naviframe);

	/* Push to naviframe */
	ugd->navi_item = elm_naviframe_item_push(ugd->naviframe, APP_STRING("IDS_ST_HEADER_APP_NOTIFICATIONS_ABB"), back_btn, NULL, ugd->list_main, NULL);
	elm_naviframe_item_pop_cb_set(ugd->navi_item, _notifiacation_setting_main_pop_cb, ugd);

}

static Evas_Object *_create_fullview(Evas_Object *parent, ug_data *ugd)
{
	NOTISET_DBG("");
	retv_if(!ugd, NULL);

	/* Create Full view */
	ugd->layout = create_layout(parent);

	elm_layout_theme_set(ugd->layout, "layout", "application", "default");
	elm_win_indicator_mode_set(parent, ELM_WIN_INDICATOR_SHOW);
 
	/* Create background */
	create_background(ugd->layout);

	/* Create navigation bar */
	ugd->naviframe = create_naviframe(ugd->layout);

		/* Create main view */
	_create_main_view(ugd);

	return ugd->layout;
}

static bool on_create(void *priv)
{
	NOTISET_DBG("START");
	ug_data *ugd = priv;
	create_app_notification_list();
	Evas_Object *win = _create_win();
	ugd->layout = _create_fullview(win, ugd);
	elm_object_content_set(win, ugd->layout);

	return true;
}

static void on_destroy(void *priv)
{
	NOTISET_DBG("");
	ug_data *ugd = priv;
	remove_all_apps_list();
	free(ugd);
}

static void on_language(app_event_info_h event_info, void *user_data)
{
	char *language_set = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &language_set);

	if (language_set)
	{
		elm_language_set(language_set);
		free(language_set);
	}
}

int main(int argc, char *argv[])
{
	NOTISET_DBG("app init !!!");

	ug_data *ugd = calloc(1, sizeof(ug_data));
	if (!ugd)
		return -1;

	ui_app_lifecycle_callback_s event_callback;
	memset(&event_callback, 0x00, sizeof(ui_app_lifecycle_callback_s));

	event_callback.create = on_create;
	event_callback.terminate = on_destroy;

	app_event_handler_h ev = NULL;
	ui_app_add_event_handler(&ev, APP_EVENT_LANGUAGE_CHANGED, on_language, ugd);

	return ui_app_main(argc, argv, &event_callback, &ugd);
}

/**
 * @}
 */

/* End of a file */

