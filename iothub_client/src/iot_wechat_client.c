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

#include <iotdm_client.h>
#include <iot_wechat_client.h>
#include <azure_c_shared_utility/xlogging.h>
#include <azure_c_shared_utility/threadapi.h>
#include <stdlib.h>

typedef struct IOT_WECHAT_CLIENT_TAG
{
    IOTDM_CLIENT_HANDLE iotdm_client_handle;
    void(* audioPlayerCallback)(const char *);
} IOT_WECHAT_CLIENT;

static void HandleUpdateSnapshot(const SHADOW_MESSAGE_CONTEXT* messageContext, const SHADOW_SNAPSHOT* snapshot, void* callbackContext);

IOT_WECHAT_CLIENT_HANDLE iot_wechat_client_init(char* address, char* device, void audioPlayCallback(const char*)) {
    IOT_WECHAT_CLIENT_HANDLE handle = malloc(sizeof(IOT_WECHAT_CLIENT));

    IOTDM_CLIENT_HANDLE dmHandle = iotdm_client_init(address, device);
    if (NULL == dmHandle)
    {
        LogError("iotdm_client_init failed");
        return NULL;
    }
    handle->iotdm_client_handle = dmHandle;
    handle->audioPlayerCallback = audioPlayCallback;

    return handle;
}

int iot_wechat_client_subscribe(const IOT_WECHAT_CLIENT_HANDLE handle, char* device, char* username, char* password) {
    iotdm_client_register_update_snapshot(handle->iotdm_client_handle, HandleUpdateSnapshot, handle);

    IOTDM_CLIENT_OPTIONS options;
    options.cleanSession = true;
    options.clientId = device;
    options.username = username;
    options.password = password;
    options.keepAliveInterval = 5;
    options.retryTimeoutInSeconds = 300;

    if (0 != iotdm_client_connect(handle->iotdm_client_handle, &options))
    {
        iotdm_client_deinit(handle->iotdm_client_handle);
        LogError("iotdm_client_connect failed");
        return __FAILURE__;
    }

    LogInfo("iotdm client connect success");

    while (iotdm_client_dowork(handle->iotdm_client_handle) >= 0)
    {
        ThreadAPI_Sleep(100);
    }
    return 0;
}

void iot_wechat_client_deinit(IOT_WECHAT_CLIENT_HANDLE handle) {
    iotdm_client_deinit(handle->iotdm_client_handle);
}

static void HandleUpdateSnapshot(const SHADOW_MESSAGE_CONTEXT* messageContext, const SHADOW_SNAPSHOT* snapshot, void* callbackContext)
{
    LogInfo("Received a message for shadow update snapshot.");
    LogInfo("Request ID:");
    LogInfo(messageContext->requestId);
    LogInfo("Device:");
    LogInfo(messageContext->device);

    JSON_Value* value = json_object_get_wrapping_value(snapshot->reported);
    JSON_Object* jsonObject = json_value_get_object(value);
    const char * audioUrl = json_object_dotget_string(jsonObject, "audioUrl");
    char* encoded = json_serialize_to_string(value);
    LogInfo("Message:");
    LogInfo(encoded);

    if (NULL == callbackContext)
    {
        LogError("Failure: the callback context is NULL.");
    }
    else {
        ((IOT_WECHAT_CLIENT_HANDLE) callbackContext)->audioPlayerCallback(audioUrl);
    }
}


