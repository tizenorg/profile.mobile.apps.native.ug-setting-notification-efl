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
#include <time.h>
#include <notification_setting_internal.h>

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
#define WEEK_MAX_STRING 560
#define DAY_MAX_LENGTH 80

#define GREEN_TEXT_COLOR "<font_size=80><color=#b3b3b3>%s</color></font_size>"
#define GREY_TEXT_COLOR "<font_size=80><color=#97e57b>%s</color></font_size>"

#define GREEN_TEXT_MAIN "<color=#b3b3b3>%s</color>"
#define GREY_TEXT_MAIN "<color=#97e57b>%s</color>"

typedef struct changecolor
{
    dnd_schedule_week_flag_e week;
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
    bool is_start_time;
} datetime_s;

datetime_s start_time_p, end_time_p;
static int day = 0;

static void set_set_schedule(bool state);
static void enable_time_items(bool enable);
static Evas_Object *create_week_repeat_layout(Evas_Object* parent);
static void week_button_clicked_cb(void *data, Evas_Object *obj, void *event_info);
static Evas_Object *create_week_button(Evas_Object *parent, const char *text, dnd_schedule_week_flag_e week);
static Evas_Object *repeat_weekly_layout_cb(Evas_Object* parent, void *data);
static void popup_cancel_btn_clicked_cb(void *data , Evas_Object *obj , void *event_info);
static void popup_set_btn_clicked_cb(void *data , Evas_Object *obj , void *event_info);
static void create_datetime_popup(datetime_s *dt);
static void launch_popup_cb(void *data , Evas_Object *obj , void *event_info);
static Evas_Object *create_time_button(Evas_Object *parent, const char *text, const char *format, datetime_s *dt);
static Evas_Object *create_start_end_time_layout(Evas_Object* parent);
static void close_set_schedule_cb(void *data, Evas_Object *obj, void *event_info);
static const char *make_color_text(dnd_schedule_week_flag_e week_day, const char *text);

const char *get_time_string()
{
    char buff_start[TIME_STRING_SIZE / 2] = {0};
    char buff_end[TIME_STRING_SIZE / 2] = {0};
    char buff[TIME_STRING_SIZE] = {0};

    time_t local_time = time(0);
    struct tm *time_info = localtime(&local_time);

    start_time_p.saved_time = *time_info;
    start_time_p.saved_time.tm_sec = 0;
    end_time_p.saved_time = *time_info;
    end_time_p.saved_time.tm_sec = 0;

    notification_system_setting_h system_setting = NULL;
    int err = notification_system_setting_load_system_setting(&system_setting);
    if (err != NOTIFICATION_ERROR_NONE || system_setting == NULL)
    {
        NOTISET_ERR("notification_system_setting_load_system_setting failed [%d]\n", err);
        return NULL;
    }
    notification_system_setting_dnd_schedule_get_start_time(system_setting, &start_time_p.saved_time.tm_hour, &start_time_p.saved_time.tm_min);
    if(start_time_p.saved_time.tm_hour == 0 && start_time_p.saved_time.tm_min == 0)
    {
        start_time_p.saved_time.tm_hour = 22;
        notification_system_setting_dnd_schedule_set_start_time(system_setting, start_time_p.saved_time.tm_hour, start_time_p.saved_time.tm_min);
    }
    notification_system_setting_dnd_schedule_get_end_time(system_setting, &end_time_p.saved_time.tm_hour, &end_time_p.saved_time.tm_min);
    if(end_time_p.saved_time.tm_hour == 0 && end_time_p.saved_time.tm_min == 0)
    {
        end_time_p.saved_time.tm_hour = 8;
        notification_system_setting_dnd_schedule_set_start_time(system_setting, end_time_p.saved_time.tm_hour, end_time_p.saved_time.tm_min);
    }

    notification_system_setting_update_system_setting(system_setting);
    if (err != NOTIFICATION_ERROR_NONE)
        NOTISET_ERR("notification_setting_update_setting [%d]\n", err);
    if(system_setting)
        notification_system_setting_free_system_setting(system_setting);


    if(get_time_format() == time_format_12H)
    {
        strftime(buff_start, TIME_STRING_SIZE / 2, TIME_12_FORMAT, &start_time_p.saved_time);
        strftime(buff_end, TIME_STRING_SIZE / 2, TIME_12_FORMAT, &end_time_p.saved_time);
    }
    else
    {
        strftime(buff_start, TIME_STRING_SIZE / 2, TIME_24_FORMAT, &start_time_p.saved_time);
        strftime(buff_end, TIME_STRING_SIZE / 2, TIME_24_FORMAT, &end_time_p.saved_time);
    }

    snprintf(buff, TIME_STRING_SIZE, "%s ~ %s %s", buff_start, buff_end, is_next_day() ? APP_STRING("IDS_ST_SBODY_NEXT_DAY_M_LC_ABB") : "");
    return strdup(buff);
}

static const char *make_color_text(dnd_schedule_week_flag_e week_day, const char *text)
{
    char variable[DAY_MAX_LENGTH] = { 0 };
    if((day & week_day) == 0)
        snprintf(variable, DAY_MAX_LENGTH, GREY_TEXT_MAIN, text);
    else
        snprintf(variable, DAY_MAX_LENGTH, GREEN_TEXT_MAIN, text);

    return strdup(variable);
}

const char *get_day_string()
{
    char buff[WEEK_MAX_STRING] = { 0 };

    notification_system_setting_h system_setting = NULL;
    int err = notification_system_setting_load_system_setting(&system_setting);
    if(err != NOTIFICATION_ERROR_NONE || system_setting == NULL)
    {
        NOTISET_ERR("notification_system_setting_load_system_setting failed [%d]\n", err);
        return NULL;
    }

    notification_system_setting_dnd_schedule_get_day(system_setting, &day);
    if(system_setting)
        notification_system_setting_free_system_setting(system_setting);

    strcat(buff, make_color_text(DND_SCHEDULE_WEEK_FLAG_MONDAY, APP_STRING("WDS_ALM_BUTTON_M_M_MONDAY_ABB")));
    strcat(buff, " ");
    strcat(buff, make_color_text(DND_SCHEDULE_WEEK_FLAG_TUESDAY, APP_STRING("WDS_ALM_BUTTON_T_M_TUESDAY_ABB")));
    strcat(buff, " ");
    strcat(buff, make_color_text(DND_SCHEDULE_WEEK_FLAG_WEDNESDAY, APP_STRING("WDS_ALM_BUTTON_W_M_WEDNESDAY_ABB")));
    strcat(buff, " ");
    strcat(buff, make_color_text(DND_SCHEDULE_WEEK_FLAG_THURSDAY, APP_STRING("WDS_ALM_BUTTON_T_M_THURSDAY_ABB")));
    strcat(buff, " ");
    strcat(buff, make_color_text(DND_SCHEDULE_WEEK_FLAG_FRIDAY, APP_STRING("WDS_ALM_BUTTON_F_M_FRIDAY_ABB")));
    strcat(buff, " ");
    strcat(buff, make_color_text(DND_SCHEDULE_WEEK_FLAG_SATURDAY, APP_STRING("WDS_ALM_BUTTON_S_M_SATURDAY_ABB")));
    strcat(buff, " ");
    strcat(buff, make_color_text(DND_SCHEDULE_WEEK_FLAG_SUNDAY, APP_STRING("WDS_ALM_BUTTON_S_M_SUNDAY_ABB")));

    return strdup(buff);
}

bool is_next_day()
{
    double diff_time = difftime(mktime(&end_time_p.saved_time), mktime(&start_time_p.saved_time));
    NOTISET_DBG("diff_time = %f", diff_time);
    return diff_time <= 0 ? true : false;
}

static void set_set_schedule(bool state)
{
    NOTISET_TRACE_BEGIN;

    notification_system_setting_h system_setting = NULL;
    int err = notification_system_setting_load_system_setting(&system_setting);
    if(err != NOTIFICATION_ERROR_NONE || system_setting == NULL)
    {
        NOTISET_ERR("notification_system_setting_load_system_setting failed [%d]\n", err);
        return;
    }
    notification_system_setting_dnd_schedule_set_enabled(system_setting, state);
    NOTISET_DBG("set set_schedule [%d]\n", state);

    err = notification_system_setting_update_system_setting(system_setting);
    if(err != NOTIFICATION_ERROR_NONE)
        NOTISET_ERR("notification_setting_update_setting [%d]\n", err);

    if(system_setting)
        notification_system_setting_free_system_setting(system_setting);
}

static void enable_time_items(bool enable)
{
    ug_data *ug_main = get_app_ui_data();
    ret_if(ug_main == NULL);
    unsigned int i = 1;
    unsigned int size = elm_genlist_items_count(ug_main->list_sub);
    Elm_Object_Item *item = elm_genlist_first_item_get(ug_main->list_sub);
    for(; i < size; ++i)
    {
        Elm_Object_Item *next = elm_genlist_item_next_get(item);
        elm_object_item_disabled_set(next, !enable);
        item = next;
    }
}

void set_schedule_check_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
    NOTISET_TRACE_BEGIN;
    ug_data *ug_main = get_app_ui_data();
    ret_if(ug_main == NULL);
    bool check = elm_check_state_get(obj);
    enable_time_items(check);
    set_set_schedule(check);
    elm_genlist_item_update(elm_genlist_item_prev_get(elm_genlist_last_item_get(ug_main->list_main)));
}

int get_time_format()
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

    return genlist;
}

bool get_schedule()
{
    NOTISET_TRACE_BEGIN;

    bool set_schedule = false;
    notification_system_setting_h system_setting = NULL;
    int err = notification_system_setting_load_system_setting(&system_setting);
    if (err != NOTIFICATION_ERROR_NONE || system_setting == NULL)
    {
        NOTISET_ERR("notification_system_setting_load_system_setting failed [%d]\n", err);
        return set_schedule;
    }
    notification_system_setting_dnd_schedule_get_enabled(system_setting, &set_schedule);
    NOTISET_DBG("set_schedule [%d]\n", set_schedule);

    if(system_setting)
        notification_system_setting_free_system_setting(system_setting);

    return set_schedule;
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
    ug_data *ug_main = get_app_ui_data();
    ret_if(ug_main == NULL);
    changecolor_s *cc = data;
    char buf[BUTTON_TEXT_SIZE] = {0, };

    if(cc->change_color)
    {
        day ^= cc->week;
        snprintf(buf, sizeof(buf), GREY_TEXT_COLOR, cc->text);
        elm_object_text_set(cc->label, buf);
    }
    else
    {
        day |= cc->week;
        snprintf(buf, sizeof(buf), GREEN_TEXT_COLOR, cc->text);
        elm_object_text_set(cc->label, buf);
    }
    cc->change_color = !cc->change_color;

    notification_system_setting_h system_setting = NULL;
    int err = notification_system_setting_load_system_setting(&system_setting);
    if (err != NOTIFICATION_ERROR_NONE || system_setting == NULL)
    {
        NOTISET_ERR("notification_system_setting_load_system_setting failed [%d]\n", err);
        return;
    }
    notification_system_setting_dnd_schedule_set_day(system_setting, day);
    notification_system_setting_update_system_setting(system_setting);
    if(system_setting)
        notification_system_setting_free_system_setting(system_setting);

    elm_genlist_item_update(elm_genlist_item_prev_get(elm_genlist_last_item_get(ug_main->list_main)));
}

static Evas_Object *create_week_button(Evas_Object *parent, const char *text, dnd_schedule_week_flag_e week)
{
    //add buttons
    char buf[BUTTON_TEXT_SIZE] = {0, };
    Evas_Object *button = elm_button_add(parent);
    evas_object_size_hint_max_set(button, WEEK_BUTTON_SIZE, WEEK_BUTTON_SIZE);
    evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_style_set(button, "transparent");

    Evas_Object *label = elm_label_add(button);
    evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

    notification_system_setting_h system_setting = NULL;
    int err = notification_system_setting_load_system_setting(&system_setting);
    if (err != NOTIFICATION_ERROR_NONE || system_setting == NULL)
    {
        NOTISET_ERR("notification_system_setting_load_system_setting failed [%d]\n", err);
        return NULL;
    }
    notification_system_setting_dnd_schedule_get_day(system_setting, &day);
    if(system_setting)
        notification_system_setting_free_system_setting(system_setting);

    changecolor_s *cc = calloc(1, sizeof(changecolor_s));
    cc->week = week;
    cc->label = label;
    cc->text = text;

    if((day & week) == 0)
    {
        snprintf(buf, sizeof(buf), GREY_TEXT_COLOR, text);
        cc->change_color = false;
    }
    else
    {
        snprintf(buf, sizeof(buf), GREEN_TEXT_COLOR, text);
        cc->change_color = true;
    }
    elm_object_text_set(label, buf);
    evas_object_show(label);

    evas_object_smart_callback_add(button, "clicked", week_button_clicked_cb, cc);
    elm_object_content_set(button, label);
    evas_object_show(button);
    elm_box_pack_end(parent, button);
    return button;
}

static Evas_Object *repeat_weekly_layout_cb(Evas_Object* parent, void *data)
{
    Evas_Object *layout = create_week_repeat_layout(parent);
    char buf[MAX_TEXT_SIZE] = {0, };
    // Set text into layout
    Evas_Object *text = elm_label_add(layout);
    elm_label_ellipsis_set(text, EINA_TRUE);
    evas_object_size_hint_weight_set(text, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(text, EVAS_HINT_FILL, EVAS_HINT_FILL);
    snprintf(buf, sizeof(buf), "<font_size=40>%s</font_size>", APP_STRING("IDS_ST_BODY_REPEAT_WEEKLY_ABB"));
    elm_object_text_set(text, strdup(buf));
    elm_object_part_content_set(layout, "elm.swallow.text", text);
    evas_object_show(text);

    Evas_Object *box = elm_box_add(layout);

    evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_box_horizontal_set(box, EINA_TRUE);
    elm_object_part_content_set(layout, "elm.box.content", box);

    create_week_button(box, APP_STRING("WDS_ALM_BUTTON_M_M_MONDAY_ABB"), DND_SCHEDULE_WEEK_FLAG_MONDAY);
    create_week_button(box, APP_STRING("WDS_ALM_BUTTON_T_M_TUESDAY_ABB"), DND_SCHEDULE_WEEK_FLAG_TUESDAY);
    create_week_button(box, APP_STRING("WDS_ALM_BUTTON_W_M_WEDNESDAY_ABB"), DND_SCHEDULE_WEEK_FLAG_WEDNESDAY);
    create_week_button(box, APP_STRING("WDS_ALM_BUTTON_T_M_THURSDAY_ABB"), DND_SCHEDULE_WEEK_FLAG_THURSDAY);
    create_week_button(box, APP_STRING("WDS_ALM_BUTTON_F_M_FRIDAY_ABB"), DND_SCHEDULE_WEEK_FLAG_FRIDAY);
    create_week_button(box, APP_STRING("WDS_ALM_BUTTON_S_M_SATURDAY_ABB"), DND_SCHEDULE_WEEK_FLAG_SATURDAY);
    create_week_button(box, APP_STRING("WDS_ALM_BUTTON_S_M_SUNDAY_ABB"), DND_SCHEDULE_WEEK_FLAG_SUNDAY);

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
    ug_data *ug_main = get_app_ui_data();
    ret_if(ug_main == NULL);
    char buff[TIME_STRING_SIZE] = { 0 };
    const char *format;
    datetime_s *dt = data;

    elm_datetime_value_get(dt->datetime, &dt->saved_time);
    format = elm_datetime_format_get(dt->datetime);

    notification_system_setting_h system_setting = NULL;
    int err = notification_system_setting_load_system_setting(&system_setting);
    if(err != NOTIFICATION_ERROR_NONE || system_setting == NULL)
    {
        NOTISET_ERR("notification_system_setting_load_system_setting failed [%d]\n", err);
        return;
    }

    if(dt->saved_time.tm_hour == start_time_p.saved_time.tm_hour && dt->saved_time.tm_min == start_time_p.saved_time.tm_min)
        notification_system_setting_dnd_schedule_set_start_time(system_setting, dt->saved_time.tm_hour, dt->saved_time.tm_min);
    else
        notification_system_setting_dnd_schedule_set_end_time(system_setting, dt->saved_time.tm_hour, dt->saved_time.tm_min);

    err = notification_system_setting_update_system_setting(system_setting);
    if(err != NOTIFICATION_ERROR_NONE)
        NOTISET_ERR("notification_setting_update_setting [%d]\n", err);

    if(system_setting)
        notification_system_setting_free_system_setting(system_setting);

    if(!strcmp(format, POPUP_TIME_12_FORMAT))
        strftime(buff, TIME_STRING_SIZE, TIME_12_FORMAT, &dt->saved_time);
    else
        strftime(buff, TIME_STRING_SIZE, TIME_24_FORMAT, &dt->saved_time);

    elm_genlist_item_update(elm_genlist_last_item_get(ug_main->list_sub));
    elm_genlist_item_update(elm_genlist_item_prev_get(elm_genlist_last_item_get(ug_main->list_main)));

    elm_object_text_set(dt->button, buff);

    evas_object_del(dt->popup);
}

static void create_datetime_popup(datetime_s *dt)
{
    Evas_Object *set_btn, *cancel_btn;
    ug_data *ug_main = get_app_ui_data();
    ret_if(ug_main == NULL);

    dt->popup = elm_popup_add(ug_main->naviframe);
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
    ug_data *ug_main = get_app_ui_data();
    ret_if(ug_main == NULL);

    dt->datetime = elm_datetime_add(ug_main->naviframe);

    format = evas_object_data_get(obj, "format");
    bool fmt12hours = !strcmp(format, TIME_12_FORMAT);
    const char *fmt = fmt12hours ? TIME_12_LAYOUT : TIME_24_LAYOUT;
    const char* timeMask = fmt12hours ? POPUP_TIME_12_FORMAT : POPUP_TIME_24_FORMAT;

    elm_object_style_set(dt->datetime, fmt);
    elm_datetime_format_set(dt->datetime, timeMask);

    elm_datetime_value_set(dt->datetime, &dt->saved_time);
    evas_object_size_hint_align_set(dt->datetime, EVAS_HINT_FILL, EVAS_HINT_FILL);

    evas_object_show(dt->datetime);
    elm_box_pack_end(box, dt->datetime);
    elm_object_content_set(dt->popup, box);

    evas_object_show(dt->popup);
}

static Evas_Object *create_time_button(Evas_Object *parent, const char *text, const char *format, datetime_s *dt)
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

Evas_Object* start_end_time_item(Evas_Object* parent, bool is_start_time_item)
{
    Evas_Object* layout = create_start_end_time_layout(parent);
    char buff[TIME_STRING_SIZE] = {0};
    datetime_s dt;
    time_t local_time = time(0);
    struct tm *time_info = localtime(&local_time);

    dt.is_start_time = is_start_time_item;
    dt.saved_time = *time_info;
    dt.saved_time.tm_sec = 0;

    notification_system_setting_h system_setting = NULL;
    int err = notification_system_setting_load_system_setting(&system_setting);
    if(err != NOTIFICATION_ERROR_NONE || system_setting == NULL)
    {
        NOTISET_ERR("notification_system_setting_load_system_setting failed [%d]\n", err);
        return NULL;
    }
    if(is_start_time_item)
    {
        notification_system_setting_dnd_schedule_get_start_time(system_setting, &dt.saved_time.tm_hour, &dt.saved_time.tm_min);
        if(dt.saved_time.tm_hour == 0 && dt.saved_time.tm_min == 0)
        {
            dt.saved_time.tm_hour = 22;
            notification_system_setting_dnd_schedule_set_start_time(system_setting, dt.saved_time.tm_hour, dt.saved_time.tm_min);
        }
    }
    else
    {
        notification_system_setting_dnd_schedule_get_end_time(system_setting, &dt.saved_time.tm_hour, &dt.saved_time.tm_min);
        if(dt.saved_time.tm_hour == 0 && dt.saved_time.tm_min == 0)
        {
            dt.saved_time.tm_hour = 8;
            notification_system_setting_dnd_schedule_set_end_time(system_setting, dt.saved_time.tm_hour, dt.saved_time.tm_min);
        }
    }

    err = notification_system_setting_update_system_setting(system_setting);
    if(err != NOTIFICATION_ERROR_NONE)
        NOTISET_ERR("notification_setting_update_setting [%d]\n", err);

    if(system_setting)
        notification_system_setting_free_system_setting(system_setting);

    long int curr_end_time = (long int)mktime(&end_time_p.saved_time);
    long int curr_start_time = (long int)mktime(&start_time_p.saved_time);
    if(dt.is_start_time && curr_start_time == -1)
        start_time_p = dt;
    else if(!dt.is_start_time && curr_end_time == -1)
        end_time_p = dt;

    char *timeFormat = get_time_format() == time_format_12H ? TIME_12_FORMAT : TIME_24_FORMAT;
    if(is_start_time_item)
    {
        strftime(buff, TIME_STRING_SIZE, timeFormat, &start_time_p.saved_time);
        start_time_p.button = create_time_button(layout, buff, timeFormat, &start_time_p);
    }
    else
    {
        strftime(buff, TIME_STRING_SIZE, timeFormat, &end_time_p.saved_time);
        end_time_p.button = create_time_button(layout, buff, timeFormat, &end_time_p);
    }

    return layout;
}

static void close_set_schedule_cb(void *data, Evas_Object *obj, void *event_info)
{
    NOTISET_TRACE_BEGIN;
    ret_if(!data);
    elm_naviframe_item_pop(data);
}

void gl_set_schedule_selected(ug_data *data)
{
    NOTISET_TRACE_BEGIN;
    ug_data *ugd = data;
    ret_if(!ugd);

    /* back Button */
    Evas_Object *back_btn = elm_button_add(ugd->naviframe);
    elm_object_style_set(back_btn, "naviframe/back_btn/default");
    evas_object_smart_callback_add(back_btn, "clicked", close_set_schedule_cb, ugd->naviframe);

    /* Push to naviframe */
    ugd->list_sub = _create_set_schedule_disturb_gl(ugd);
    append_gl_start_option(ugd->list_sub, "type1", "set-schedule");
    append_gl_full_item(ugd->list_sub, repeat_weekly_layout_cb, NULL);
    append_gl_start_option(ugd->list_sub, "type1", "start-time");
    append_gl_start_option(ugd->list_sub, "type1", "end-time");
    ugd->navi_item = elm_naviframe_item_push(ugd->naviframe, APP_STRING("IDS_ST_MBODY_SET_SCHEDULE_M_TIME"), back_btn, NULL, ugd->list_sub, NULL);
    enable_time_items(get_schedule());
}
