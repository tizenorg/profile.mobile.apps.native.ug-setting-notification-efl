/*
 * Copyright (c) 2009 - 2016 Samsung Electronics Co., Ltd. All rights reserved.
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

#include "allowed-calls.h"

extern ug_data g_ug_data;

static char *items[4] = {
    "IDS_QP_TAB4_ALL_M_RECEIVED_ALL",
    "IDS_CST_OPT_CONTACTS_ONLY",
    "IDS_ST_MBODY_FAVOURITE_CONTACTS_ONLY_ABB",
    "IDS_QP_TAB4_NONE_M_BLOCEKD"
};

static void popup_hide_cb(void *data, Evas_Object *obj, void *event_info)
{
    evas_object_del(obj);
}

static char *gl_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
    int index = (int)data;
    return strdup(APP_STRING(items[index]));
}

static void popup_btn_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
    NOTISET_TRACE_BEGIN;
    Evas_Object *popup = data;
    //TODO: need to be implemented after API appear
    evas_object_del(popup);
}

static void create_reject_calls_popup(const char *title_text, const char *content)
{
    Evas_Object *popup;
    Evas_Object *layout;
    Evas_Object *btn;
    Evas_Object *scroller;
    Evas_Object *label;
    Evas_Object *check;
    Evas_Object *win = g_ug_data.win;

    popup = elm_popup_add(win);
    elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
    elm_object_part_text_set(popup, "title,text", APP_STRING(title_text));
    evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

    eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, popup_hide_cb, NULL);
    evas_object_smart_callback_add(popup, "dismissed", popup_hide_cb, NULL);
    evas_object_smart_callback_add(popup, "block,clicked", popup_hide_cb, NULL);

    /* layout */
    layout = elm_layout_add(popup);
    char *res_path = app_get_resource_path();
    if(res_path)
    {
        char edj_path[PATH_MAX] = { 0, };
        snprintf(edj_path, PATH_MAX, "%s%s", res_path, "edje/reject_calls_popup.edj");
        elm_layout_file_set(layout, edj_path, "popup_checkview_layout");
        free(res_path);
    }
    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_part_text_set(layout, "elm.text", "Description text");

    /* ok button */
    btn = elm_button_add(popup);
    elm_object_style_set(btn, "bottom");
    elm_object_text_set(btn, APP_STRING("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));
    elm_object_part_content_set(popup, "button1", btn);
    evas_object_smart_callback_add(btn, "clicked", popup_btn_clicked_cb, popup);

    /* check */
    check = elm_check_add(popup);
    elm_object_text_set(check, APP_STRING("IDS_ST_OPT_DONT_SHOW_AGAIN_ABB"));
    elm_object_part_content_set(layout, "elm.swallow.end", check);

    /* scroller */
    scroller = elm_scroller_add(layout);
    elm_scroller_bounce_set(scroller, EINA_TRUE, EINA_TRUE);
    elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
    elm_object_part_content_set(layout, "elm.swallow.content", scroller);

    /* label */
    label = elm_label_add(scroller);
    elm_object_style_set(label, "popup/default");
    elm_label_line_wrap_set(label, ELM_WRAP_MIXED);
    elm_object_text_set(label, APP_STRING(content));
    evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(label);
    elm_object_content_set(scroller, label);

    elm_object_content_set(popup, layout);

    evas_object_show(popup);
}

static void all_item_clicked()
{
    NOTISET_TRACE_BEGIN;
    //TODO: need to be implemented after API appear
}

static void contacts_only_clicked()
{
    NOTISET_TRACE_BEGIN;
    create_reject_calls_popup("IDS_ST_HEADER_REJECT_CALLS_ABB2",
            "IDS_ST_BODY_INCOMING_CALLS_FROM_CONTACTS_YOU_HAVE_NOT_ALLOWED_WILL_BE_REJECTED_AUTOMATICALLY");
    //TODO: need to be implemented after API appear
}

static void favorite_contacts_only_clicked()
{
    NOTISET_TRACE_BEGIN;
    create_reject_calls_popup("IDS_ST_HEADER_REJECT_CALLS_ABB2",
            "IDS_ST_BODY_INCOMING_CALLS_FROM_CONTACTS_YOU_HAVE_NOT_ALLOWED_WILL_BE_REJECTED_AUTOMATICALLY");
    //TODO: need to be implemented after API appear
}

static void none_item_clicked()
{
    NOTISET_TRACE_BEGIN;
    create_reject_calls_popup("IDS_ST_HEADER_REJECT_ALL_CALLS_ABB",
            "IDS_ST_POP_ALL_INCOMING_CALLS_WILL_BE_REJECTED_AUTOMATICALLY");
    //TODO: need to be implemented after API appear
}

static void gl_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *popup = data;
    int index = 0;
    Elm_Object_Item *item = event_info;

    index = (int)elm_object_item_data_get(item);
    switch (index)
    {
        case 0:
            all_item_clicked();
            break;
        case 1:
            contacts_only_clicked();
            break;
        case 2:
            favorite_contacts_only_clicked();
            break;
        case 3:
            none_item_clicked();
            break;
    }
    evas_object_del(popup);
}

static void create_allow_calls_from_popup()
{
    NOTISET_TRACE_BEGIN;
    static Elm_Genlist_Item_Class itc;
    Evas_Object *popup;
    Evas_Object *box;
    Evas_Object *genlist;
    int i;
    Evas_Object *win = g_ug_data.win;

    popup = elm_popup_add(win);
    elm_popup_orient_set(popup, ELM_POPUP_ORIENT_BOTTOM);
    elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
    elm_object_part_text_set(popup, "title,text", APP_STRING("IDS_ST_HEADER_ALLOW_CALLS_FROM_ABB"));
    evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(popup, EVAS_HINT_FILL, EVAS_HINT_FILL);

    eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, popup_hide_cb, NULL);
    evas_object_smart_callback_add(popup, "dismissed", popup_hide_cb, NULL);
    evas_object_smart_callback_add(popup, "block,clicked", popup_hide_cb, NULL);

    /* box */
    box = elm_box_add(popup);
    elm_box_homogeneous_set(box, EINA_FALSE);
    evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(box);
    elm_object_content_set(popup, box);

    /* genlist */
    genlist = elm_genlist_add(box);

    itc.item_style = "1line";;
    itc.func.text_get = gl_text_get_cb;
    itc.func.content_get = NULL;
    itc.func.state_get = NULL;
    itc.func.del = NULL;

    for (i = 0; i < 4; i++) {
        elm_genlist_item_append(genlist, &itc, (void *)i, NULL, ELM_GENLIST_ITEM_NONE, gl_sel_cb, popup);
    }

    evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_genlist_mode_set(genlist, ELM_LIST_EXPAND);
    evas_object_show(genlist);
    elm_box_pack_end(box, genlist);

    evas_object_show(popup);
}

void gl_allowed_calls_selected()
{
    create_allow_calls_from_popup();
}
