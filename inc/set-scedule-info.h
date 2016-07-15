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


#ifndef __SET_SCHEDULE_INFO_H__
#define __SET_SCHEDULE_INFO_H__

#include <Evas.h>

#include <Elementary.h>
#include "log.h"
#include "common-efl.h"

/**
 * @brief Call back when schedule state is changed
 */
void set_schedule_check_changed_cb(void *data, Evas_Object *obj, void *event_info);
/**
 * @brief Create full set schedule view
 * @param[in] data User created ug data
 */
void gl_set_schedule_selected(ug_data *data);
/**
 * @brief Get state of schedule
 * @retval Returns true if schedule is enabled, else false
 */
bool get_schedule();
/**
 * @brief Create start or end time item with time picker
 * @param[in] parent Genlist
 * @param[in] is_start_time_item  Set true if is start time item, false if end time
 * @retval Returns layout with time item
 */
Evas_Object* start_end_time_item(Evas_Object* parent, bool is_start_time_item);

/**
 * @brief Check if end time is earlier then start time
 * @retval Returns true if end time is earlier, else false
 */
bool is_next_day();
/**
 * @brief Forms a string with the end and start time of schedule
 * @retval Returns string
 */
const char *get_time_string();
/**
 * @brief Forms a string with the selected days of the week
 * @retval Returns string
 */
const char *get_day_string();
/**
 * @brief Get time format from system settings
 * @retval Returns 12H or 24H time format
 */
int get_time_format();

#endif //__SET_SCHEDULE_INFO_H__
