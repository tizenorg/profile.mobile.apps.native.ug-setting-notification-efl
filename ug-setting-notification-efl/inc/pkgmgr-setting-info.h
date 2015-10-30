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


#ifndef __PKGMGR_SETTING_INFO_H__
#define __PKGMGR_SETTING_INFO_H__

#include <tizen.h>
#include "log.h"


#define DEFAULT_ICON IMAGEDIR"/unknown.png"


typedef struct {
	/* innate features */
	char *appid;
	char *name;
	char *icon;

	int ordering;
	bool is_app_data;
	int index;
	bool allow_to_notify;
	bool do_not_disturb_except;
	bool visibility_class;
	int item_style;
} item_info_s;

HAPI item_info_s *apps_item_info_create(const char *appid);
HAPI char *get_app_icon(char *appid);
HAPI char *get_app_name(const char *appid);
HAPI int apps_sort_cb(const void *d1, const void *d2);


#endif //__PKGMGR_SETTING_INFO_H__
