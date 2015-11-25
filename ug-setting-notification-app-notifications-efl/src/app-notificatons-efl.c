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
#include <ui-gadget-module.h>
#include <package_manager.h>
#include <tzplatform_config.h>
#include "notification-setting-info.h"
#include "common-efl.h"

/**
 * @{
 */

ug_data *g_ug_data = NULL;

static Eina_Bool _notifiacation_setting_main_pop_cb(void *data, Elm_Object_Item * it)
{
	NOTISET_DBG("");
	ug_data *ugd = (ug_data *) data;
	retv_if(!ugd, EINA_FALSE);

	ug_destroy_me(ugd->ug);

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

	g_ug_data = ugd;
	
	/* Create genlist */
	g_ug_data->list_main = _create_app_notification_gl(ugd);

	/* back Button */
	Evas_Object *back_btn = elm_button_add(ugd->naviframe);
	elm_object_style_set(back_btn, "naviframe/back_btn/default");
	evas_object_smart_callback_add(back_btn, "clicked", back_button_cb, ugd->naviframe);

	/* Push to naviframe */
	g_ug_data->navi_item = elm_naviframe_item_push(ugd->naviframe, APP_STRING("IDS_ST_HEADER_APP_NOTIFICATIONS_ABB"), back_btn, NULL, g_ug_data->list_main, NULL);
	elm_naviframe_item_pop_cb_set(g_ug_data->navi_item, _notifiacation_setting_main_pop_cb, ugd);

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

static void *on_create(ui_gadget_h ug, enum ug_mode mode, app_control_h service, void *priv)
{

	NOTISET_DBG("START");
	Evas_Object *parent;
	ug_data *ugd;

	if (!ug || !priv)
		return NULL;

	ugd = priv;
	ugd->ug = ug;

	parent = ug_get_parent_layout(ug);
	if (!parent)
		return NULL;

	// set mode
	ugd->mode = 0;

	create_app_notification_list();
	ugd->layout = _create_fullview(parent, ugd);

	return ugd->layout;


}

static void on_start(ui_gadget_h ug, app_control_h service, void *priv)
{
	NOTISET_DBG("");
	/* Fill me */
}

static void on_pause(ui_gadget_h ug, app_control_h service, void *priv)
{
	NOTISET_DBG("");
	/* Fill me */
}

static void on_resume(ui_gadget_h ug, app_control_h service, void *priv)
{
	NOTISET_DBG("");
}

static void on_destroy(ui_gadget_h ug, app_control_h service, void *priv)
{
	NOTISET_DBG("");

	ug_data *ugd = priv;;

	remove_all_apps_list();

	EVAS_OBJECT_DELIF(ugd->cancel_button);
	EVAS_OBJECT_DELIF(ugd->done_button);
	EVAS_OBJECT_DELIF(ugd->list_sub);
	EVAS_OBJECT_DELIF(ugd->list_main);
	EVAS_OBJECT_DELIF(ugd->naviframe);
	EVAS_OBJECT_DELIF(ugd->layout);

}

static void on_message(ui_gadget_h ug, app_control_h msg, app_control_h service, void *priv)
{
	/* Fill me */
}

static void on_event(ui_gadget_h ug, enum ug_event event, app_control_h service, void *priv)
{
	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
		break;
	case UG_EVENT_REGION_CHANGE:
		break;
	case UG_EVENT_ROTATE_PORTRAIT:
		break;
	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
		break;
	default:
		break;
	}
}

static void on_key_event(ui_gadget_h ug, enum ug_key_event event, app_control_h service, void *priv)
{
	switch (event) {
	case UG_KEY_EVENT_NONE:
		break;
	case UG_KEY_EVENT_END:
		/**
		 * @TODO: Terminate UG itself?
		 */
		break;
	default:
		break;
	}
}

int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	ug_data *ugd;

	if (ops == NULL) {
		return -EINVAL;
	}

	ugd = calloc(1, sizeof(ug_data));

	if (!ugd) {
		return -ENOMEM;
	}

	ops->create = on_create;
	ops->start = on_start;
	ops->pause = on_pause;
	ops->resume = on_resume;
	ops->destroy = on_destroy;
	ops->message = on_message;
	ops->event = on_event;
	ops->key_event = on_key_event;
	ops->priv = ugd;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	if (bindtextdomain(PKGNAME, tzplatform_mkpath(TZ_SYS_RO_APP, "ug-setting-notification-efl/res/locale")) == NULL) {
		/* Failed to allocate memory for allocating heap for store the path of LOCALE directory */
	}

	return 0;
}

void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	NOTISET_DBG("");

	ug_data *ugd;

	if (ops == NULL) {
		return;
	}

	ugd = ops->priv;

	free(ugd);
}

/**
 * @}
 */

/* End of a file */

