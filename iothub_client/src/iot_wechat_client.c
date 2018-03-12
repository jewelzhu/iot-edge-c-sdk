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
#include <iothub_mqtt_client.h>
#include <azure_c_shared_utility/utf8_checker.h>
#include <mach/boolean.h>
#include <azure_c_shared_utility/buffer_.h>

typedef struct IOT_WECHAT_CLIENT_TAG
{
    IOTDM_CLIENT_HANDLE iotdm_client_handle;
    void(* audioPlayerCallback)(const char *);
    IOTHUB_MQTT_CLIENT_HANDLE iothub_mqtt_client_handle;
    char* deviceId;
} IOT_WECHAT_CLIENT;

#define MESSAGE_ID_TAG  "msgId"
#define SEQ_TAG "seq"
#define IS_FINISH_TAG "isEnd"
#define DEVICE_ID_TAG "devId"
#define BOUNDARY "\r\n--\r\n"
#define MQTT_MSG_BODY_LIMIT_IN_BYTES 32000

static void HandleUpdateSnapshot(const SHADOW_MESSAGE_CONTEXT* messageContext, const SHADOW_SNAPSHOT* snapshot, void* callbackContext);


static void on_recv_callback(MQTT_MESSAGE_HANDLE msgHandle, void* context)
{
    const APP_PAYLOAD* appMsg = mqttmessage_getApplicationMsg(msgHandle);
    IOTHUB_MQTT_CLIENT_HANDLE clientHandle = (IOTHUB_MQTT_CLIENT_HANDLE)context;

    LogInfo("Incoming Msg: Packet Id: %d\r\nTopic Name: %s\r\nIs Retained: %s\r\nIs Duplicate: %s\r\nApp Msg: ", mqttmessage_getPacketId(msgHandle),
                 mqttmessage_getTopicName(msgHandle),
                 mqttmessage_getIsRetained(msgHandle) ? "true" : "fale",
                 mqttmessage_getIsDuplicateMsg(msgHandle) ? "true" : "fale"
    );

    // when receive message is "stop", call destroy method to exit
    // trigger stop by send a message to topic "msgA" and payload with "stop"
    if (strcmp((const char *)appMsg->message, "stop") == 0)
    {
        LogInfo("disconnect");
        iothub_mqtt_disconnect(clientHandle);
    }
}


IOT_WECHAT_CLIENT_HANDLE iot_wechat_client_init(char* subAddress, char* subUsername, char* subPassword,
                                                void audioPlayCallback(const char*), char* pubAddress, char* pubUsername,
                                                char* pubPassword, char* deviceId) {
    IOT_WECHAT_CLIENT_HANDLE handle = malloc(sizeof(IOT_WECHAT_CLIENT));

    IOTDM_CLIENT_HANDLE dmHandle = iotdm_client_init(subAddress, deviceId, false);
    if (NULL == dmHandle)
    {
        LogError("iotdm_client_init failed");
        return NULL;
    }
    iotdm_client_register_update_snapshot(dmHandle, HandleUpdateSnapshot, handle);

    IOTDM_CLIENT_OPTIONS dmOptions;
    dmOptions.cleanSession = true;
    dmOptions.clientId = deviceId;
    dmOptions.username = subUsername;
    dmOptions.password = subPassword;
    dmOptions.keepAliveInterval = 5;
    dmOptions.retryTimeoutInSeconds = 300;

    if (0 != iotdm_client_connect(dmHandle, &dmOptions))
    {
        iotdm_client_deinit(dmHandle);
        LogError("iotdm_client_connect failed");
        return NULL;
    }

    LogInfo("iotdm client connect success");
    handle->iotdm_client_handle = dmHandle;
    handle->audioPlayerCallback = audioPlayCallback;


    MQTT_CLIENT_OPTIONS options = { 0 };
    options.clientId = deviceId;
    options.willMessage = NULL;
    options.willTopic = NULL;
    options.username = pubUsername;
    options.password = pubPassword;
    options.keepAliveInterval = 10;
    options.useCleanSession = true;
    options.qualityOfServiceValue = DELIVER_AT_MOST_ONCE;

    MQTT_CONNECTION_TYPE type = MQTT_CONNECTION_TLS;

    IOTHUB_CLIENT_RETRY_POLICY retryPolicy = IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF;

    size_t retryTimeoutLimitInSeconds = 1000;
    IOTHUB_MQTT_CLIENT_HANDLE clientHandle = initialize_mqtt_client_handle(&options, pubAddress, type, on_recv_callback,
                                                                           retryPolicy, retryTimeoutLimitInSeconds);
    if (clientHandle == NULL)
    {
        LogError("Error: fail to initialize IOTHUB_MQTT_CLIENT_HANDLE");
        return NULL;
    }

    int result = iothub_mqtt_doconnect(clientHandle, 60);

    if (result == __FAILURE__)
    {
        LogError("fail to establish connection with server");
        return NULL;
    }

    handle->iothub_mqtt_client_handle = clientHandle;
    handle->deviceId = deviceId;
    return handle;
}

// publishDataSize must <= 32000
int iot_wechat_client_pub_data(const IOT_WECHAT_CLIENT_HANDLE handle, char* pubTopic, const unsigned char* publishData, size_t publishDataSize) {
    if (publishDataSize > MQTT_MSG_BODY_LIMIT_IN_BYTES) {
        LogError("size of a mqtt msg cannot exceed 32K");
        return __FAILURE__;
    }
    int result = publish_mqtt_message(handle->iothub_mqtt_client_handle, pubTopic, DELIVER_AT_LEAST_ONCE, publishData,
                                  publishDataSize, NULL, NULL);

    if (result == __FAILURE__) {
        LogError("pub data to mqtt fail");
    }

    return result;
}

int iot_wechat_client_pub_voice(const IOT_WECHAT_CLIENT_HANDLE handle, char* pubTopic, const unsigned char* voiceData,
                                size_t voidDataSzie, char * messageId, int seq, int finish) {
    BUFFER_HANDLE buffer =  BUFFER_new();
    if (buffer == NULL) {
        LogError("create buffer fail");
        return __FAILURE__;
    }
    JSON_Value* json = json_value_init_object();
    JSON_Object* root = json_object(json);
    json_object_set_string(root, MESSAGE_ID_TAG, messageId);
    json_object_set_number(root, SEQ_TAG, seq);
    json_object_set_number(root, IS_FINISH_TAG, finish);
    json_object_set_string(root, DEVICE_ID_TAG, handle->deviceId);
    char* encodedJson = json_serialize_to_string(json);
    int result;
    if (BUFFER_append_build(buffer, (const unsigned char*)encodedJson, strlen(encodedJson)) != 0) {
        LogError("oom");
        result = __FAILURE__;
    }
    else if (BUFFER_append_build(buffer, (const unsigned char*)BOUNDARY, strlen(BOUNDARY)) != 0) {
        LogError("oom");
        result = __FAILURE__;
    }
    else if (BUFFER_append_build(buffer, voiceData, voidDataSzie) != 0) {
        LogError("oom");
        result = __FAILURE__;
    }
    else {
        result = iot_wechat_client_pub_data(handle, pubTopic, BUFFER_u_char(buffer), BUFFER_length(buffer));
    }

    json_object_clear(root);
    free(encodedJson);
    BUFFER_delete(buffer);

    return result;
}

int iot_wechat_client_subscribe(const IOT_WECHAT_CLIENT_HANDLE handle) {
    while (iotdm_client_dowork(handle->iotdm_client_handle) >= 0)
    {
        ThreadAPI_Sleep(100);
    }
    return 0;
}

void iot_wechat_client_deinit(IOT_WECHAT_CLIENT_HANDLE handle) {
    iotdm_client_deinit(handle->iotdm_client_handle);
    iothub_mqtt_disconnect(handle->iothub_mqtt_client_handle);
    iothub_mqtt_destroy(handle->iothub_mqtt_client_handle);
    free(handle);
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


