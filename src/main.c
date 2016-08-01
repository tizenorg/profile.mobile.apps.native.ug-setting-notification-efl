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
#include "do-not-disturb-efl.h"
#include "pkgmgr-setting-info.h"
#include "excepted-apps-efl.h"
#include "common-efl.h"

/* TODO: change when app control operation will be add */
#define NOTIFICATIONS_ON_LOCK_SCREEN_OP "http://tizen.org/appcontrol/operation/setting/notification_on_lock_screen"
#define DO_NOT_DISTURB_OP "http://tizen.org/appcontrol/operation/setting/do_not_disturb"
#define APP_NOTIFICATIONS_OP "http://tizen.org/appcontrol/operation/setting/app_notifications"

#define WHITE_COLOR 255, 255, 255, 255

typedef enum
{
	NOTIF_APP_TYPE,
	DO_NOT_DISTURB_APP_TYPE,
	NOTIFICATION_ON_LOCK_SCREEN_APP_TYPE
} app_type;


static Evas_Object *_create_win()
{
	/* Window: */
	Evas_Object *win = elm_win_add(NULL, "", ELM_WIN_BASIC);
	elm_win_conformant_set(win, EINA_TRUE);
	elm_win_autodel_set(win, EINA_TRUE);
	elm_win_indicator_mode_set(win, ELM_WIN_INDICATOR_SHOW);
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
    NOTISET_TRACE_BEGIN;
	ug_data *ugd = (ug_data *) data;
	retv_if(!ugd, EINA_FALSE);

	ui_app_exit();

	return EINA_FALSE;
}

static Evas_Object* _create_do_not_disturb_gl(ug_data *ugd)
{
	Evas_Object *parent = ugd->naviframe;

	Evas_Object *genlist = elm_genlist_add(parent);
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

	do_not_disturb_append_item_in_list(genlist);

	return genlist;
}

Evas_Object* _create_lockscreen_content(ug_data *ugd)
{
    /* box */
    Evas_Object *box = elm_box_add(ugd->naviframe);
    elm_box_homogeneous_set(box, EINA_FALSE);
    evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);

    Evas_Object *bg = create_background(box);
    evas_object_size_hint_align_set(bg, EVAS_HINT_FILL, EVAS_HINT_FILL);

    Evas_Object *layout = create_layout(bg);
    evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_part_content_set(bg, "elm.swallow.content", layout);
    elm_box_pack_end(box, bg);

    /* Lower genlist */
    Evas_Object *lower_genlist = elm_genlist_add(box);
    append_gl_radio_item_list(lower_genlist, get_lower_noti_list(), "default");
    evas_object_size_hint_weight_set(lower_genlist, EVAS_HINT_EXPAND, 0.5);
    evas_object_size_hint_align_set(lower_genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_genlist_mode_set(lower_genlist, ELM_LIST_COMPRESS);
    evas_object_show(lower_genlist);
    elm_box_pack_end(box, lower_genlist);

    evas_object_show(box);

    return box;
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

	append_gl_allow_all(genlist);
	append_gl_start_option(genlist, "multiline", "app-notifications");
	append_gl_item_list(genlist, get_all_apps_list(), ITEM_STYLE_TYPE_ONE);

	return genlist;
}

static void _create_do_not_disturb_view(void *data)
{
    NOTISET_TRACE_BEGIN;
    ug_data *ugd = (ug_data *) data;
    ret_if(!ugd);

    create_do_not_disturb_application_list();
    ugd->list_main = _create_do_not_disturb_gl(ugd);

    /* back Button */
    Evas_Object *back_btn = elm_button_add(ugd->naviframe);
    elm_object_style_set(back_btn, "naviframe/back_btn/default");
    evas_object_smart_callback_add(back_btn, "clicked", back_button_cb, ugd->naviframe);

    /* Push to naviframe */
    ugd->navi_item = elm_naviframe_item_push(ugd->naviframe, APP_STRING("IDS_ST_HEADER_DO_NOT_DISTURB_ABB"), back_btn, NULL, ugd->list_main, NULL);
    elm_naviframe_item_pop_cb_set(ugd->navi_item, _notifiacation_setting_main_pop_cb, ugd);
}

static void _create_notif_on_lock_screen_view(void *data)
{
    NOTISET_TRACE_BEGIN;
    ug_data *u_data = (ug_data *)data;
    ret_if(!u_data);

    create_notification_on_ls_list();

    /* Create genlist */
    Evas_Object *box = _create_lockscreen_content(u_data);

    /* Push to naviframe */
    u_data->navi_item = elm_naviframe_item_push(u_data->naviframe, strdup(APP_STRING("IDS_QP_HEADER_LOCK_SCREEN_CONTENT_ABB")), NULL, NULL, box, NULL);
    elm_naviframe_item_pop_cb_set(u_data->navi_item, _notifiacation_setting_main_pop_cb, u_data);

    /* Title Cancel Button */
    Evas_Object *cancel_btn = elm_button_add(u_data->naviframe);
    elm_object_style_set(cancel_btn, "naviframe/title_left");
    evas_object_smart_callback_add(cancel_btn, "clicked", cancel_button_noti_ls_cb, u_data);
    elm_object_text_set(cancel_btn, APP_STRING("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
    elm_object_item_part_content_set(u_data->navi_item, "title_left_btn", cancel_btn);
    u_data->cancel_button = cancel_btn;

    /* Title Done Button */
    Evas_Object *done_btn = elm_button_add(u_data->naviframe);
    elm_object_style_set(done_btn, "naviframe/title_right");
    evas_object_smart_callback_add(done_btn, "clicked", done_button_noti_ls_cb, u_data);
    elm_object_text_set(done_btn, APP_STRING("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));
    elm_object_item_part_content_set(u_data->navi_item, "title_right_btn", done_btn);
    u_data->done_button = done_btn;
}

static void _create_notif_view(void *data)
{
    NOTISET_TRACE_BEGIN;
	ug_data *ugd = (ug_data *) data;
	ret_if(!ugd);

	create_app_notification_list();

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

static Evas_Object *_create_fullview(Evas_Object *parent, ug_data *ugd, app_type type)
{
    NOTISET_TRACE_BEGIN;
    retv_if(!ugd, NULL);

    switch(type)
    {
        case NOTIF_APP_TYPE:
            _create_notif_view(ugd);
            break;

        case DO_NOT_DISTURB_APP_TYPE:
            _create_do_not_disturb_view(ugd);
            break;

        case NOTIFICATION_ON_LOCK_SCREEN_APP_TYPE:
            _create_notif_on_lock_screen_view(ugd);
            break;

        default:
            NOTISET_DBG("Unknown app type");
            break;
    }

    return ugd->layout;
}

static bool on_create(void *priv)
{
    NOTISET_TRACE_BEGIN;
	ug_data *ugd = priv;
	elm_app_base_scale_set(2.6);

	/* Create window */
	ugd->win = _create_win();

	/* Create Full view */
	ugd->layout = create_layout(ugd->win);
	elm_layout_theme_set(ugd->layout, "layout", "application", "default");

	/* Create navigation bar */
	ugd->naviframe = create_naviframe(ugd->layout);

	return true;
}

static void on_destroy(void *priv)
{
    NOTISET_TRACE_BEGIN;
	ug_data *ugd = priv;
	remove_all_apps_list();
	free(ugd);
}

static void on_app_control(app_control_h app_control, void *user_data)
{
    NOTISET_TRACE_BEGIN;

    ug_data *ugd = user_data;
    char *op_str = NULL;

    app_control_get_operation(app_control, &op_str);

    if(!op_str)
    {
        NOTISET_DBG("app_control operation is null");
        return;
    }

    NOTISET_DBG("%s", op_str);

    app_type type = NOTIF_APP_TYPE;
    if(strcmp(DO_NOT_DISTURB_OP, op_str) == 0)
    {
        type = DO_NOT_DISTURB_APP_TYPE;
    }
    else if(strcmp(NOTIFICATIONS_ON_LOCK_SCREEN_OP, op_str) == 0)
    {
        type = NOTIFICATION_ON_LOCK_SCREEN_APP_TYPE;
    }

    ugd->layout = _create_fullview(ugd->win, ugd, type);
    elm_object_content_set(ugd->win, ugd->layout);

    free(op_str);
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
	NOTISET_TRACE_BEGIN;

	ug_data *ug_main = calloc(1, sizeof(ug_data));

	set_app_ui_data(ug_main);

	ui_app_lifecycle_callback_s event_callback;
	memset(&event_callback, 0x00, sizeof(ui_app_lifecycle_callback_s));

	event_callback.create = on_create;
	event_callback.terminate = on_destroy;
	event_callback.app_control = on_app_control;

	app_event_handler_h ev = NULL;
	ui_app_add_event_handler(&ev, APP_EVENT_LANGUAGE_CHANGED, on_language, ug_main);

	return ui_app_main(argc, argv, &event_callback, ug_main);
}
