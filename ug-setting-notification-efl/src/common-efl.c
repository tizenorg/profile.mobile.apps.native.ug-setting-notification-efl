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

#include <tzplatform_config.h>
#include "pkgmgr-setting-info.h"
#include "common-efl.h"

Evas_Object *create_layout(Evas_Object *parent)
{
	Evas_Object *layout = NULL;

	if (parent == NULL) {
		NOTISET_DBG("Parent is null.");
		return NULL;
	}

	layout = elm_layout_add(parent);
	if (layout == NULL) {
		NOTISET_DBG("Cannot add layout.");
		return NULL;
	}

	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	evas_object_show(layout);

	return layout;
}

Evas_Object *create_background(Evas_Object *parent)
{
	Evas_Object *bg = NULL;

	bg = elm_bg_add(parent);

	evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_style_set(bg, "group_list");

	elm_object_part_content_set(parent, "elm.swallow.bg", bg);
	evas_object_show(bg);

	return bg;
}

Evas_Object *create_naviframe(Evas_Object *parent)
{
	Evas_Object *naviframe = NULL;

	if (parent == NULL) {
		NOTISET_DBG("Parent is null.");
		return NULL;
	}

	naviframe = elm_naviframe_add(parent);
	if (naviframe == NULL) {
		NOTISET_DBG("Cannot add naviframe.");
		return NULL;
	}

	elm_object_part_content_set(parent, "elm.swallow.content", naviframe);

	elm_naviframe_prev_btn_auto_pushed_set(naviframe, EINA_FALSE);
	eext_object_event_callback_add(naviframe, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);

	evas_object_show(naviframe);

	return naviframe;
}

void gl_loaded_cb(void *data EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
}

void gl_realized_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
}

void gl_longpressed_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
}

void gl_selected_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event_info)
{
	Elm_Object_Item *it = (Elm_Object_Item*)event_info;
	elm_genlist_item_selected_set(it, EINA_FALSE);

	Evas_Object *check = elm_object_item_part_content_get(it, "elm.swallow.end");
	if (!check) {
		check = elm_object_item_part_content_get(it, "elm.icon.right");
	}

	if (check) {
		elm_check_state_set(check, !elm_check_state_get(check));
		evas_object_smart_callback_call(check, "changed", NULL);
	}
}

void gl_contracted_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   Elm_Object_Item *it = event_info;
   elm_genlist_item_subitems_clear(it);
}

void gl_del_cb(void *data, Evas_Object *obj EINA_UNUSED)
{
}

static Evas_Object* _create_icon(Evas_Object *parent, char* icon)
{
	Evas_Object *img = elm_image_add(parent);

	if(access(icon, 0) == -1) {
		icon = strdup(tzplatform_mkpath(TZ_SYS_RO_ICONS, "default/small/ug-setting-notification-efl.png"));
	}
	

	elm_image_file_set(img, icon, NULL);
	evas_object_color_set(img, 255, 255, 255, 255);
	evas_object_size_hint_align_set(img, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(img, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	
	return img;
}

static char *gl_group_index_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	char *text = (char*)data;


	if (!strcmp(part, "elm.text.end")) {
		return strdup(text);
	}

	return NULL;
}

void append_gl_group_index(Evas_Object *genlist, char* text) {
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	ret_if(!itc);

	itc->item_style = "group_index";
	itc->func.text_get = gl_group_index_text_get_cb;
	itc->func.content_get = NULL;
	itc->func.del = gl_del_cb;


	elm_genlist_item_append(genlist,							/* genlist object */
							itc,							/* item class */
							strdup(text),					/* item class user data */
							NULL,							/* parent item */
							ELM_GENLIST_ITEM_NONE,			/* item type */
							gl_selected_cb, 				/* select smart callback */
							NULL);						/* smart callback user data */


	elm_genlist_item_class_free(itc);
}

static void _excepted_check_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	NOTISET_DBG("");
	item_info_s *data_list = data;
	ret_if(!data_list);

	Eina_Bool state = elm_check_state_get(obj);
	data_list->do_not_disturb_except = state;

	NOTISET_DBG("appid = %s state = %d", data_list->appid, state);
}

static void _do_not_disturb_check_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	NOTISET_DBG("");

	Eina_Bool state = elm_check_state_get(obj);
	set_do_not_disturb(state);
	NOTISET_DBG("do_not_disturb check value = %s", state==false ? "FALSE":"TRUE");
}


static void _allow_to_nofity_check_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	NOTISET_DBG("");
	item_info_s *data_list = data;
	ret_if(!data_list);

	Eina_Bool state = elm_check_state_get(obj);
	set_allow_to_nofity(data_list->appid, state);
	data_list->allow_to_notify = state;
	NOTISET_DBG("appid = %s state = %d", data_list->appid, state);
}

static Evas_Object* gl_content_get_cb(void *data, Evas_Object *obj, const char *part)
{
	retv_if(!data, NULL);

	Evas_Object *content = NULL;

	item_info_s *data_list = data;
	if (!strcmp(part, "elm.swallow.icon") || !strcmp(part, "elm.icon.left") ) {
		content = elm_layout_add(obj);
		//elm_layout_theme_set(content, "layout", "list/C/type.1", "default");
		Evas_Object *icon = _create_icon(content, data_list->icon);
		elm_layout_content_set(content, "elm.swallow.content", icon);
		return content;
	}

	if (!strcmp(part, "elm.swallow.end")) {
		content = elm_check_add(obj);
		elm_object_style_set(content, "on&off");
		elm_check_state_set(content, data_list->do_not_disturb_except);
		evas_object_show(content);
		evas_object_pass_events_set(content, 1);
		evas_object_smart_callback_add(content, "changed", _excepted_check_changed_cb, data_list);
		evas_object_propagate_events_set(content, 0);
		return content;
	} else if (data_list->item_style == ITEM_STYLE_ONE_LINE && !strcmp(part, "elm.icon.right")) {
		content = elm_check_add(obj);
		elm_object_style_set(content, "on&off");
		elm_check_state_set(content, data_list->allow_to_notify);
		evas_object_show(content);
		evas_object_pass_events_set(content, 1);
		evas_object_smart_callback_add(content, "changed", _allow_to_nofity_check_changed_cb, data_list);
		evas_object_propagate_events_set(content, 0);
		return content;
	}

	return content;
}


static char* gl_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	item_info_s *data_list = data;
	retv_if(data_list == NULL, NULL);

	if (!strcmp(part, "elm.text") || !strcmp(part, "elm.text.main.left")) {
		return strdup(data_list->name);
	}

	return NULL;
}

void append_gl_item_list(Evas_Object *genlist, Eina_List* list, int style) {
	NOTISET_DBG("");

	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	ret_if(!itc);

	item_info_s *item;
	char *before_text = NULL;
	int count = 0;

	if (style == ITEM_STYLE_ONE_LINE){
		itc->item_style = "1line";
	} else if(style == ITEM_STYLE_ONE_ICON){
		itc->item_style = "one_icon";
	} else {
		itc->item_style = "default";
	}
	itc->func.text_get = gl_text_get_cb;
	itc->func.content_get = gl_content_get_cb;
	itc->func.del = gl_del_cb; 

	count = eina_list_count(list);
	NOTISET_DBG("count %d", count);

	while (list) {
		item = (item_info_s*)eina_list_data_get(list);
		item->item_style = style;

		if (style == ITEM_STYLE_ONE_LINE) {
			int unicode_len = eina_unicode_utf8_get_len(item->name);
			int str_len = strlen(item->name);
			int lang_byte = str_len/unicode_len+(str_len%unicode_len > 0 ? 1: 0);

			if ( before_text == NULL || strncmp(before_text, item->name, lang_byte) != 0 ) {
				if (before_text != NULL) free(before_text);
				before_text = strndup(item->name, lang_byte);
				append_gl_group_index(genlist, before_text);
			}
		}
		elm_genlist_item_append(genlist,						/* genlist object */
								itc,							/* item class */
								item,							/* item class user data */
								NULL,							/* parent item */
								ELM_GENLIST_ITEM_NONE,			/* item type */
						 		gl_selected_cb, 				/* select smart callback */
								NULL);						/* smart callback user data */
		list = eina_list_next(list);
	}
	elm_genlist_item_class_free(itc);
	if (before_text)
		free(before_text);
}


static char *_gl_option_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];

	retv_if(!data, NULL);
	if (!strcmp(data, "app-notifications")) {
		if (!strcmp("elm.text.multiline", part))
		{
			if (get_do_not_disturb()) { 
				snprintf(buf, sizeof(buf), "<font_size=30>%s</font_size>",
					APP_STRING("IDS_ST_BODY_DO_NOT_DISTURB_IS_ENABLED_NOTIFICATIONS_FROM_THE_APPLICATIONS_YOU_HAVE_SELECTED_BELOW_WILL_ONLY_BE_SHOWN_IN_THE_NOTIFICATION_PANEL"));
			} else {
				snprintf(buf, sizeof(buf), "<font_size=30>%s</font_size>",
					APP_STRING("IDS_ST_BODY_SELECT_THE_APPLICATIONS_YOU_WANT_TO_RECEIVE_NOTIFICATIONS_FROM_BELOW"));
			}
			return strdup(buf);
		}
	} else if(!strcmp(data, "do-not-disturb")) {
		if (!strcmp("elm.text.multiline", part)) {
			snprintf(buf, sizeof(buf), "<font_size=30>%s</font_size>",
				APP_STRING("IDS_ST_BODY_MUTE_ALL_ALERTS_AND_SHOW_NOTIFICATIONS_IN_THE_NOTIFICATION_PANEL_ONLY_AND_NOT_IN_POP_UPS_OR_ON_THE_STATUS_BAR_MSG"));
			return strdup(buf);
		}

		if (!strcmp(part, "elm.text")) {
			return strdup(APP_STRING("IDS_ST_HEADER_DO_NOT_DISTURB_ABB"));
		}
	}
	
	return NULL;
}



static Evas_Object* _gl_option_content_get_cb(void *data, Evas_Object *obj, const char *part)
{
	retv_if(!data, NULL);

	if (!strcmp(data, "do-not-disturb") &&!strcmp(part, "elm.swallow.end") ) {
		Evas_Object *check = NULL;
		check = elm_check_add(obj);
		elm_object_style_set(check, "on&off");
		elm_check_state_set(check, get_do_not_disturb());
		evas_object_show(check);
		evas_object_pass_events_set(check, 1);
		evas_object_smart_callback_add(check, "changed", _do_not_disturb_check_changed_cb, NULL);
		evas_object_propagate_events_set(check, 0); 
		return check;
	}
	return NULL;
}



void append_gl_start_option(Evas_Object *genlist, char *style, char *ugName) {
	NOTISET_DBG("");

	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	ret_if(!itc);

	itc->item_style = style;
	itc->func.text_get = _gl_option_text_get_cb;
	itc->func.content_get = _gl_option_content_get_cb;
	itc->func.del = gl_del_cb;
	
	elm_genlist_item_append(genlist,						/* genlist object */
							itc,							/* item class */
							ugName,							/* item class user data */
							NULL,							/* parent item */
							ELM_GENLIST_ITEM_NONE,			/* item type */
							gl_selected_cb, 				/* select smart callback */
							NULL);						/* smart callback user data */

	elm_genlist_item_class_free(itc);
}


void back_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(!data);
	elm_naviframe_item_pop(data);
}
