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


#include <package_manager.h>
#include <stdlib.h>

#include "pkgmgr-setting-info.h"

int apps_sort_cb(const void *d1, const void *d2)
{
	item_info_s *tmp1 = (item_info_s *) d1;
	item_info_s *tmp2 = (item_info_s *) d2;

	if (NULL == tmp1 || NULL == tmp1->name) return 1;
	else if (NULL == tmp2 || NULL == tmp2->name) return -1;

	return strcmp(tmp1->name, tmp2->name);
}

HAPI char *get_app_icon(char *appid)
{
	char *icon_path = NULL;
	char *package_id = NULL;
	package_manager_get_package_id_by_app_id(appid, &package_id);
	if (package_id) {

		package_info_h package_info = NULL;
		package_manager_get_package_info(package_id, &package_info);
		if (!package_info)
			return NULL;

		package_info_get_icon(package_info, &icon_path);
		package_info_destroy(package_info);
	}
	free(package_id);
	return icon_path;
}


HAPI char *get_app_name(const char *appid)
{
	char *name = NULL;
	char *package_id = NULL;
	package_manager_get_package_id_by_app_id(appid, &package_id);
	if (package_id) {

		package_info_h package_info = NULL;
		package_manager_get_package_info(package_id, &package_info);
		if (!package_info)
			return NULL;

		package_info_get_label(package_info, &name);
		package_info_destroy(package_info);
	}
	free(package_id);
	return name;
}
