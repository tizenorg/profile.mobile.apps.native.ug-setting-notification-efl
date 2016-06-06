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

#include "set-scedule-info.h"
#include <system_settings.h>

extern ug_data g_ug_data;

enum TimeFormat
    {
        time_format_unknown,
        time_format_24H,
        time_format_12H
    };

#define TIME_12_FORMAT "%I:%M %p"
#define POPUP_TIME_12_FORMAT "%d/%b/%Y %I:%M %p"
#define TIME_12_LAYOUT "time_layout"

#define POPUP_TIME_24_FORMAT "%d/%b/%Y %H:%M"
#define TIME_24_FORMAT "%H:%M"
#define TIME_24_LAYOUT "time_layout_24hr"

#define TIME_STRING_SIZE 200
#define BUTTON_TEXT_SIZE 512
#define WEEK_BUTTON_SIZE 80

typedef struct changecolor
{
    const char *text;
    bool change_color;
    Evas_Object *label;
} changecolor_s;

typedef struct datetime
{
    Evas_Object *button;
    Evas_Object *datetime;
    Evas_Object *popup;
    struct tm saved_time;
    bool is_start_button;
} datetime_s;

static int get_time_format()
{
    bool timeFormat = false;
    if(system_settings_get_value_bool(SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR, &timeFormat) < 0)
        return time_format_unknown;
    return timeFormat ? time_format_24H : time_format_12H;
}

static Evas_Object *_create_set_schedule_disturb_gl(ug_data *ugd)
{
    NOTISET_TRACE_BEGIN;
    Evas_Object *parent = ugd->naviframe;

    Evas_Object *genlist = elm_genlist_add(parent);
    elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
    evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);

    /* Add Smart Callback */
    evas_object_smart_callback_add(genlist, "realized", gl_realized_cb, NULL);
    evas_object_smart_callback_add(genlist, "loaded", gl_loaded_cb, NULL);
    evas_object_smart_callback_add(genlist, "longpressed", gl_longpressed_cb, NULL);
    evas_object_smart_callback_add(genlist, "contracted", gl_contracted_cb, NULL);

    return genlist;
}

bool get_schedule()
{
    NOTISET_TRACE_BEGIN;
    // TODO: will be implemented after receiving of API
    return false;
}

static Evas_Object *create_week_repeat_layout(Evas_Object* parent)
{
    Evas_Object *layout = elm_layout_add(parent);

    char *res_path = app_get_resource_path();
    if(res_path)
    {
        char edj_path[PATH_MAX] = { 0, };
        snprintf(edj_path, PATH_MAX, "%s%s", res_path, "edje/repeat_weekly_item.edj");
        elm_layout_file_set(layout, edj_path, "weekly_item");
        free(res_path);
    }

    evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show(layout);

    return layout;
}
static void week_button_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
    NOTISET_TRACE_BEGIN;
    changecolor_s *cc = data;
    char buf[BUTTON_TEXT_SIZE] = {0, };

    if(cc->change_color)
    {
        snprintf(buf, sizeof(buf), "<font_size=80><color=#000000>%s</color></font_size>", cc->text);
        elm_object_text_set(cc->label, buf);
    } else {
        snprintf(buf, sizeof(buf), "<font_size=80><color=#66ff66>%s</color></font_size>", cc->text);
        elm_object_text_set(cc->label, buf);
    }

    cc->change_color = !cc->change_color;
}

static Evas_Object *create_week_button(Evas_Object *parent, const char *text)
{
    //add buttons
    Evas_Object *button = elm_button_add(parent);
    evas_object_size_hint_max_set(button, WEEK_BUTTON_SIZE, WEEK_BUTTON_SIZE);
    evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_style_set(button, "transparent");

    Evas_Object *label = elm_label_add(button);
    evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

    char buf[BUTTON_TEXT_SIZE] = {0, };
    snprintf(buf, sizeof(buf), "<font_size=80><color=#000000>%s</color></font_size>", text);
    elm_object_text_set(label, buf);
    evas_object_show(label);

    changecolor_s *cc = calloc(1, sizeof(changecolor_s));
    cc->label = label;
    cc->change_color = false;
    cc->text = text;

    evas_object_smart_callback_add(button, "clicked", week_button_clicked_cb, cc);
    elm_object_content_set(button, label);
    evas_object_show(button);
    elm_box_pack_end(parent, button);
    return button;
}

static Evas_Object *repeat_weekly_layout_cb(Evas_Object* parent, void *data)
{
    Evas_Object *layout = create_week_repeat_layout(parent);

    // Set text into layout
    Evas_Object *text = elm_label_add(layout);
    evas_object_size_hint_weight_set(text, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(text, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_text_set(text, "<font_size=40>Repeat weekly</font_size>");
    elm_object_part_content_set(layout, "elm.swallow.text", text);
    evas_object_show(text);

    Evas_Object *box = elm_box_add(layout);

    evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_box_horizontal_set(box, EINA_TRUE);
    elm_object_part_content_set(layout, "elm.box.content", box);

    create_week_button(box, "M");
    create_week_button(box, "T");
    create_week_button(box, "W");
    create_week_button(box, "T");
    create_week_button(box, "F");
    create_week_button(box, "S");
    create_week_button(box, "S");

    evas_object_show(box);
    elm_box_recalculate(box);

    return layout;
}

static void popup_cancel_btn_clicked_cb(void *data , Evas_Object *obj , void *event_info)
{
    evas_object_del(data);
}

static void popup_set_btn_clicked_cb(void *data , Evas_Object *obj , void *event_info)
{
    NOTISET_TRACE_BEGIN;
    char buff[TIME_STRING_SIZE] = {0};
    const char *format;
    datetime_s *dt = data;
    elm_datetime_value_get(dt->datetime, &dt->saved_time);
    format = elm_datetime_format_get(dt->datetime);

    if (!strcmp(format, POPUP_TIME_12_FORMAT)) {
        strftime(buff, TIME_STRING_SIZE, TIME_12_FORMAT, &dt->saved_time);
        elm_object_text_set(dt->button, buff);
    } else {
        strftime(buff, TIME_STRING_SIZE, TIME_24_FORMAT, &dt->saved_time);
        elm_object_text_set(dt->button, buff);
    }

    evas_object_del(dt->popup);
}

static void create_datetime_popup(datetime_s *dt)
{
    Evas_Object *set_btn, *cancel_btn;

    dt->popup = elm_popup_add(g_ug_data.naviframe);
    eext_object_event_callback_add(dt->popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);
    evas_object_size_hint_weight_set(dt->popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_popup_align_set(dt->popup, ELM_NOTIFY_ALIGN_FILL, 0.5);

    cancel_btn = elm_button_add(dt->popup);
    elm_object_style_set(cancel_btn, "popup");
    elm_object_text_set(cancel_btn, "Cancel");
    elm_object_part_content_set(dt->popup, "button1", cancel_btn);
    evas_object_smart_callback_add(cancel_btn, "clicked", popup_cancel_btn_clicked_cb, dt->popup);

    set_btn = elm_button_add(dt->popup);
    elm_object_style_set(set_btn, "popup");
    elm_object_text_set(set_btn, "Set");
    elm_object_part_content_set(dt->popup, "button2", set_btn);
    evas_object_smart_callback_add(set_btn, "clicked", popup_set_btn_clicked_cb, dt);
}

static void launch_popup_cb(void *data , Evas_Object *obj , void *event_info)
{
    char *format;
    datetime_s *dt = data;
    create_datetime_popup(dt);
    Evas_Object *box = elm_box_add(dt->popup);

    dt->datetime = elm_datetime_add(g_ug_data.naviframe);

    format = evas_object_data_get(obj, "format");
    bool fmt12hours = !strcmp(format, TIME_12_FORMAT);
    const char *fmt = fmt12hours ? TIME_12_LAYOUT : TIME_24_LAYOUT;
    const char* timeMask = fmt12hours ? POPUP_TIME_12_FORMAT : POPUP_TIME_24_FORMAT;

    elm_object_part_text_set(dt->popup, "title,text", "Time Picker");
    elm_object_style_set(dt->datetime, fmt);
    elm_datetime_format_set(dt->datetime, timeMask);

    elm_datetime_value_set(dt->datetime, &dt->saved_time);
    evas_object_size_hint_align_set(dt->datetime, EVAS_HINT_FILL, EVAS_HINT_FILL);

    evas_object_show(dt->datetime);
    elm_box_pack_end(box, dt->datetime);
    elm_object_content_set(dt->popup, box);

    evas_object_show(dt->popup);
}

static Evas_Object *create_time_button(Evas_Object *parent, char *text, char *format, datetime_s *dt)
{
    Evas_Object *button = elm_button_add(parent);
    elm_object_text_set(button, text);
    evas_object_show(button);
    evas_object_data_set(button, "format", format);
    evas_object_smart_callback_add(button, "clicked", launch_popup_cb, dt);
    elm_object_part_content_set(parent, "elm.swallow.content", button);
    evas_object_show(parent);

    return button;
}

static Evas_Object *create_start_end_time_layout(Evas_Object* parent)
{
    Evas_Object *layout = elm_layout_add(parent);

    char *res_path = app_get_resource_path();
    if(res_path)
    {
        char edj_path[PATH_MAX] = { 0, };
        snprintf(edj_path, PATH_MAX, "%s%s", res_path, "edje/time_button_layout.edj");
        elm_layout_file_set(layout, edj_path, "button_layout");
        free(res_path);
    }

    evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show(layout);
    return layout;
}

Evas_Object *start_end_time_item(Evas_Object* parent)
{
    Evas_Object *layout = create_start_end_time_layout(parent);

    char buff[TIME_STRING_SIZE] = {0};
    time_t local_time = time(0);
    struct tm *time_info = localtime(&local_time);

    datetime_s *dt = calloc(1, sizeof(datetime_s));
    dt->saved_time = *time_info;

    if(get_time_format() == time_format_12H) {
        strftime(buff, TIME_STRING_SIZE, TIME_12_FORMAT, &dt->saved_time);
        dt->button = create_time_button(layout, buff, TIME_12_FORMAT, dt);
    } else if(get_time_format() == time_format_24H) {
        strftime(buff, TIME_STRING_SIZE, TIME_24_FORMAT, &dt->saved_time);
        dt->button = create_time_button(layout, buff, TIME_24_FORMAT, dt);
    }

    return layout;
}

void gl_set_schedule_selected(ug_data *data)
{
    NOTISET_TRACE_BEGIN;
    ug_data *ugd = data;
    ret_if(!ugd);

    /* back Button */
    Evas_Object *back_btn = elm_button_add(ugd->naviframe);
    elm_object_style_set(back_btn, "naviframe/back_btn/default");
    evas_object_smart_callback_add(back_btn, "clicked", back_button_cb, ugd->naviframe);

    /* Push to naviframe */
    ugd->list_sub = _create_set_schedule_disturb_gl(ugd);
    append_gl_start_option(ugd->list_sub, "type1", "set-schedule");
    append_gl_full_item(ugd->list_sub, repeat_weekly_layout_cb, NULL);
    append_gl_start_option(ugd->list_sub, "type1", "start-time");
    append_gl_start_option(ugd->list_sub, "type1", "end-time");
    ugd->navi_item = elm_naviframe_item_push(ugd->naviframe, APP_STRING("IDS_ST_MBODY_SET_SCHEDULE_M_TIME"), back_btn, NULL, ugd->list_sub, NULL);
}

