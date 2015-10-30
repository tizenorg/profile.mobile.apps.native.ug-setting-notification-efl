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

#ifndef __EXCEPTED_APPS_EFL_H__
#define __EXCEPTED_APPS_EFL_H__

#include <Evas.h>
#include <stdbool.h>
#include <notification_setting.h>

#include <Elementary.h>
#include <package-manager.h>
#include <pkgmgr-info.h>



void exception_application_clicked_cb(void *data, Evas_Object *obj, void *event_info);


#endif //__EXCEPTED_APPS_EFL_H__