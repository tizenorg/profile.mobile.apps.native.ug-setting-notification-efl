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

void set_schedule_check_changed_cb(void *data, Evas_Object *obj, void *event_info);
void gl_set_schedule_selected(ug_data *data);
bool get_schedule();
Evas_Object *start_end_time_item(Evas_Object* parent);

#endif //__SET_SCHEDULE_INFO_H__
