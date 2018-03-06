/*
* Copyright (c) 2017 Baidu, Inc. All Rights Reserved.
*
* Licensed to the Apache Software Foundation (ASF) under one or more
* contributor license agreements.  See the NOTICE file distributed with
* this work for additional information regarding copyright ownership.
* The ASF licenses this file to You under the Apache License, Version 2.0
* (the "License"); you may not use this file except in compliance with
* the License.  You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef IOT_WECHAT_CLIENT_H
#define IOT_WECHAT_CLIENT_H

#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <azure_c_shared_utility/gballoc.h>

#endif // __cplusplus

typedef struct IOT_WECHAT_CLIENT_TAG* IOT_WECHAT_CLIENT_HANDLE;

MOCKABLE_FUNCTION(, IOT_WECHAT_CLIENT_HANDLE, iot_wechat_client_init, char*, broker, char*, name, void, audioPlayCallback(const char*));
MOCKABLE_FUNCTION(, int, iot_wechat_client_subscribe, IOT_WECHAT_CLIENT_HANDLE, handle, char*, device, char*, username, char*, password);

MOCKABLE_FUNCTION(, void, iot_wechat_client_deinit, IOT_WECHAT_CLIENT_HANDLE, handle);


#endif //IOT_WECHAT_CLIENT_H
