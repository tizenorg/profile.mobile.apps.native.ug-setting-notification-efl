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

#include "do-not-disturb-efl.h"
#include "excepted-apps-efl.h"
#include "common-efl.h"

#define ALLOWED_LAYOUT_NAME "allow_noti_from_layout"
#define ALLOWED_LAYOUT_TEXT_PART "elm.text"
#define ALLOWED_LAYOUT_BUTTON_PART "button"
#define ALLOWED_LAYOUT_GRID_PART "grid"

#define APP_LAYOUT_NAME "allowed_app_layout"
#define APP_LAYOUT_ICON_PART "icon"
#define APP_LAYOUT_TEXT_PART "text"
#define APP_LAYOUT_HEIGHT 191
#define APP_LAYOUT_PAD 30
#define APP_LAYOUT_HEADER 95 + 64
#define ITEMS_PER_ROW 5

extern ug_data g_ug_data;

void do_not_disturb_append_item_in_list(Evas_Object* genlist)
{
    append_gl_start_option(genlist, "multiline", "do-not-disturb");
//    TODO: append when API will be implemented, will be move to final release
    append_gl_start_option(genlist, "multiline", "set-schedule-multiline");
//    append_gl_start_option(genlist, "multiline", "allowed-calls");
    Eina_List *allowed_list = eina_list_merge(get_first_excepted_apps_list(), get_excepted_apps_list());
    append_gl_full_item(genlist, do_not_disturb_allowed_apps_cont_cb, allowed_list);
}

Evas_Object *do_not_disturb_allowed_apps_cont_cb(Evas_Object* parent, void *data)
{
    Evas_Object *full_layout = create_custom_layout(parent, ALLOWED_LAYOUT_NAME);
    elm_object_part_text_set(full_layout, ALLOWED_LAYOUT_TEXT_PART, APP_STRING("IDS_QP_HEADER_ALLOW_NOTIFICATIONS_FROM_ABB"));

    Evas_Object *select_button = elm_button_add(full_layout);
    evas_object_size_hint_weight_set(select_button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(select_button, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_text_set(select_button, APP_STRING("IDS_ST_SK_SELECT_LITE"));
    evas_object_smart_callback_add(select_button, "clicked", exception_application_clicked_cb, &g_ug_data);
    evas_object_show(select_button);

    elm_object_part_content_set(full_layout, ALLOWED_LAYOUT_BUTTON_PART,select_button);

    retv_if(!data, full_layout);
    Eina_List *allowed_apps_list = (Eina_List *)data;

    Evas_Object *table = elm_table_add(full_layout);
    elm_table_padding_set(table, 0, APP_LAYOUT_PAD);
    int count = eina_list_count(allowed_apps_list);

    int height = (APP_LAYOUT_HEIGHT + APP_LAYOUT_PAD) * (count / ITEMS_PER_ROW + (count % ITEMS_PER_ROW != 0 ? 1 : 0)) + APP_LAYOUT_HEADER;
    evas_object_size_hint_min_set(table, 0, height);

    int row_number = 0;
    while(allowed_apps_list)
    {
        int items_in_row = 0;
        while(allowed_apps_list && items_in_row < ITEMS_PER_ROW)
        {
            item_info_s *item = (item_info_s*)eina_list_data_get(allowed_apps_list);
            Evas_Object *app_layout = create_custom_layout(table, APP_LAYOUT_NAME);

            evas_object_show(app_layout);
            Evas_Object *icon = create_icon(app_layout, item->icon);
            evas_object_show(icon);
            elm_object_part_content_set(app_layout, APP_LAYOUT_ICON_PART,icon);
            elm_object_part_text_set(app_layout, APP_LAYOUT_TEXT_PART,item->name);
            elm_table_pack(table, app_layout, items_in_row, row_number, 1, 1);

            allowed_apps_list = eina_list_next(allowed_apps_list);
            items_in_row++;
        }
        row_number++;
    }

    elm_object_part_content_set(full_layout, ALLOWED_LAYOUT_GRID_PART,table);

    return full_layout;
}

