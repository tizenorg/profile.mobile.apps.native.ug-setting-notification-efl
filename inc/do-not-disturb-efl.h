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


#ifndef __DO_NOT_DISTURB_EFL_H__
#define __DO_NOT_DISTURB_EFL_H__

#include <Evas.h>
#include <stdbool.h>
#include <notification_setting.h>

#include <Elementary.h>
#include "log.h"
#include "common-efl.h"


void do_not_disturb_append_item_in_list(Evas_Object* genlist);
Evas_Object *do_not_disturb_allowed_apps_cont_cb(Evas_Object* parent, void *data);

#endif //__DO_NOT_DISTURB_EFL_H__
