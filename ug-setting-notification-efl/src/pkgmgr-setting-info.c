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
#include <pkgmgr-info.h>
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
	pkgmgrinfo_appinfo_h appinfo_h = NULL;
	char *tmp = NULL;
	char *name = NULL;

//	retv_if(0 > pkgmgrinfo_appinfo_get_appinfo(appid, &appinfo_h), NULL);
//	goto_if(PMINFO_R_OK != pkgmgrinfo_appinfo_get_icon(appinfo_h, &tmp), ERROR);

	pkgmgrinfo_appinfo_get_appinfo(appid, &appinfo_h);
	pkgmgrinfo_appinfo_get_icon(appinfo_h, &tmp);

	if (tmp) {
		name = strdup(tmp);
		goto_if(NULL == name, ERROR);
	} 

	pkgmgrinfo_appinfo_destroy_appinfo(appinfo_h);
	return name;

ERROR:
		pkgmgrinfo_appinfo_destroy_appinfo(appinfo_h);
		return NULL;
	
}


HAPI char *get_app_name(const char *appid)
{
	pkgmgrinfo_appinfo_h appinfo_h = NULL;
	char *tmp = NULL;
	char *name = NULL;

//	retv_if(0 > pkgmgrinfo_appinfo_get_appinfo(appid, &appinfo_h), NULL);
//	goto_if(PMINFO_R_OK != pkgmgrinfo_appinfo_get_label(appinfo_h, &tmp), ERROR);

	pkgmgrinfo_appinfo_get_appinfo(appid, &appinfo_h);
	pkgmgrinfo_appinfo_get_label(appinfo_h, &tmp);

	if (tmp) {
		name = strdup(tmp);
		goto_if(NULL == name, ERROR);
	}

	pkgmgrinfo_appinfo_destroy_appinfo(appinfo_h);
	return name;

ERROR:
	pkgmgrinfo_appinfo_destroy_appinfo(appinfo_h);
	return NULL;
}
