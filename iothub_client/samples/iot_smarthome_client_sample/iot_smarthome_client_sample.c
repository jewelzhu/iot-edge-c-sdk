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

#include <azure_c_shared_utility/platform.h>
#include <azure_c_shared_utility/threadapi.h>

#include "iot_smarthome_callback.h"
#include "iot_smarthome_client.h"
#include "iot_smarthome_client_sample.h"

// Should include serializer to operate shadow with device model.
#include "serializer.h"

#define         SPLIT               "--------------------------------------------------------------------------------------------"

// $puid
#define         DEVICE              "_baidu_sample_pump_instance"

// $endpointName/$puid
#define         USERNAME            "e6e0474a54554682a1ad7711f2d1dad6/_baidu_sample_pump_instance"

static bool isGateway;

BEGIN_NAMESPACE(BaiduIotDeviceSample);

DECLARE_MODEL(BaiduSamplePump,
WITH_DATA(int, FrequencyIn),
WITH_DATA(int, FrequencyOut),
WITH_DATA(int, Current),
WITH_DATA(int, Speed),
WITH_DATA(int, Torque),
WITH_DATA(int, Power),
WITH_DATA(int, DC_Bus_voltage),
WITH_DATA(int, Output_voltage),
WITH_DATA(int, Drive_temp)
);

END_NAMESPACE(BaiduIotDeviceSample);

static void Log(const char* message)
{
    printf("%s\r\n", message);
}

static void Log4Int(const int value)
{
    printf("%d\r\n", value);
}

static void LogCode(const int code)
{
    Log4Int(code);
}

static void LogVersion(const int version)
{
    Log4Int(version);
}

static void LogAcceptedMessage(const SHADOW_ACCEPTED* accepted)
{
    Log("ProfileVersion:");
    LogVersion(accepted->profileVersion);

    JSON_Value* value = json_object_get_wrapping_value(accepted->reported);
    char* encoded = json_serialize_to_string(value);
    Log("Reported:");
    Log(encoded);

    value = json_object_get_wrapping_value(accepted->desired);
    encoded = json_serialize_to_string(value);
    Log("Desired:");
    Log(encoded);

    value = json_object_get_wrapping_value(accepted->lastUpdateTime);
    encoded = json_serialize_to_string(value);
    Log("LastUpdateTime:");
    Log(encoded);
    json_free_serialized_string(encoded);
}

static void HandleAccepted(const SHADOW_MESSAGE_CONTEXT* messageContext, const SHADOW_ACCEPTED* accepted, void* callbackContext)
{
    Log("Request ID:");
    Log(messageContext->requestId);
    Log("Device:");
    Log(messageContext->device);

    if (isGateway == true) {
        Log("SubDevice:");
        Log(messageContext->subdevice);
    }

    LogAcceptedMessage(accepted);

    if (NULL == callbackContext)
    {
        LogError("Failure: the callback context is NULL.");
    }
}

static void HandleRejected(const SHADOW_MESSAGE_CONTEXT* messageContext, const SHADOW_ERROR* error, void* callbackContext)
{
    Log("Request ID:");
    Log(messageContext->requestId);
    Log("Device:");
    Log(messageContext->device);
    if (isGateway == true) {
        Log("SubDevice:");
        Log(messageContext->subdevice);
    }
    Log("Code:");
    LogCode(error->code);
    Log("Message:");
    Log(error->message);

    if (NULL == callbackContext)
    {
        LogError("Failure: the callback context is NULL.");
    }
}

static void HandleGetAccepted(const SHADOW_MESSAGE_CONTEXT* messageContext, const SHADOW_ACCEPTED* accepted, void* callbackContext)
{
    Log("Received a message for shadow get accepted.");
    HandleAccepted(messageContext, accepted, callbackContext);

    Log(SPLIT);
}

static void HandleGetRejected(const SHADOW_MESSAGE_CONTEXT* messageContext, const SHADOW_ERROR* error, void* callbackContext)
{
    Log("Received a message for shadow get rejected.");
    HandleRejected(messageContext, error, callbackContext);

    Log(SPLIT);
}

static void HandleUpdateRejected(const SHADOW_MESSAGE_CONTEXT* messageContext, const SHADOW_ERROR* error, void* callbackContext)
{
    Log("Received a message for shadow update rejected.");
    HandleRejected(messageContext, error, callbackContext);

    Log(SPLIT);
}

static void HandleUpdateAccepted(const SHADOW_MESSAGE_CONTEXT* messageContext, const SHADOW_ACCEPTED* accepted, void* callbackContext)
{
    Log("Received a message for shadow update accepted.");
    HandleAccepted(messageContext, accepted, callbackContext);

    Log(SPLIT);
}

static void HandleUpdateDocuments(const SHADOW_MESSAGE_CONTEXT* messageContext, const SHADOW_DOCUMENTS* documents, void* callbackContext)
{
    Log("Received a message for shadow update documents.");
    Log("Request ID:");
    Log(messageContext->requestId);
    Log("Device:");
    Log(messageContext->device);
    if (isGateway == true) {
        Log("SubDevice:");
        Log(messageContext->subdevice);
    }
    Log("ProfileVersion:");
    LogVersion(documents->profileVersion);

    JSON_Value* value = json_object_get_wrapping_value(documents->current);
    char* encoded = json_serialize_to_string(value);
    Log("Current:");
    Log(encoded);

    value = json_object_get_wrapping_value(documents->previous);
    encoded = json_serialize_to_string(value);
    Log("Previous:");
    Log(encoded);
    json_free_serialized_string(encoded);

    if (NULL == callbackContext)
    {
        LogError("Failure: the callback context is NULL.");
    }

    Log(SPLIT);
}

static void HandleUpdateSnapshot(const SHADOW_MESSAGE_CONTEXT* messageContext, const SHADOW_SNAPSHOT* snapshot, void* callbackContext)
{
    Log("Received a message for shadow update snapshot.");
    Log("Request ID:");
    Log(messageContext->requestId);
    Log("Device:");
    Log(messageContext->device);
    if (isGateway == true) {
        Log("SubDevice:");
        Log(messageContext->subdevice);
    }
    Log("ProfileVersion:");
    LogVersion(snapshot->profileVersion);

    JSON_Value* value = json_object_get_wrapping_value(snapshot->reported);
    char* encoded = json_serialize_to_string(value);
    Log("Reported:");
    Log(encoded);

    value = json_object_get_wrapping_value(snapshot->lastUpdateTime);
    encoded = json_serialize_to_string(value);
    Log("LastUpdateTime:");
    Log(encoded);
    json_free_serialized_string(encoded);

    if (NULL == callbackContext)
    {
        LogError("Failure: the callback context is NULL.");
    }

    Log(SPLIT);
}

static void HandleDelta(const SHADOW_MESSAGE_CONTEXT* messageContext, const JSON_Object* desired, void* callbackContext)
{
    Log("Received a message for shadow delta");
    Log("Request ID:");
    Log(messageContext->requestId);
    Log("Device:");
    Log(messageContext->device);
    if (isGateway == true) {
        Log("SubDevice:");
        Log(messageContext->subdevice);
    }
    JSON_Value* value = json_object_get_wrapping_value(desired);
    char* encoded = json_serialize_to_string(value);
    Log("Payload:");
    Log(encoded);
    json_free_serialized_string(encoded);

    if (NULL == callbackContext)
    {
        LogError("Failure: the callback context is NULL.");
    }

    // In the actual implementation, we should adjust the device status to match the control of device.
    // However, here we only sleep, and then update the device shadow (status in reported payload).

    ThreadAPI_Sleep(10);
    IOT_SH_CLIENT_HANDLE handle = (IOT_SH_CLIENT_HANDLE)callbackContext;
    int result = iot_smarthome_client_update_shadow(handle, messageContext->device, messageContext->requestId, 0, value, NULL);
    if (0 == result)
    {
        Log("Have done for the device controller request, and corresponding shadow is updated.");
    }
    else
    {
        LogError("Failure: failed to update device shadow.");
    }

    Log(SPLIT);
}

int iot_smarthome_client_run(bool isGatewayDevice)
{
    isGateway = isGatewayDevice;
    Log("The device management edge simulator is starting ...");
    if (0 != platform_init())
    {
        Log("platform_init failed");
        return __FAILURE__;
    }

    if (SERIALIZER_OK != serializer_init(NULL))
    {
        Log("serializer_init failed");
        return __FAILURE__;
    }

    IOT_SH_CLIENT_HANDLE handle = iot_smarthome_client_init(isGatewayDevice);
    if (NULL == handle)
    {
        Log("iot_smarthome_client_init failed");
        return __FAILURE__;
    }

    iot_smarthome_client_register_delta(handle, HandleDelta, handle);
    iot_smarthome_client_register_get_accepted(handle, HandleGetAccepted, handle);
    iot_smarthome_client_register_get_rejected(handle, HandleGetRejected, handle);
    iot_smarthome_client_register_update_accepted(handle, HandleUpdateAccepted, handle);
    iot_smarthome_client_register_update_rejected(handle, HandleUpdateRejected, handle);
    iot_smarthome_client_register_update_documents(handle, HandleUpdateDocuments, handle);
    iot_smarthome_client_register_update_snapshot(handle, HandleUpdateSnapshot, handle);

    IOT_SH_CLIENT_OPTIONS options;
    options.cleanSession = true;
    options.clientId = DEVICE;
    options.username = USERNAME;
    options.keepAliveInterval = 5;
    options.retryTimeoutInSeconds = 300;

    if (0 != iot_smarthome_client_connect(handle, &options))
    {
        iot_smarthome_client_deinit(handle);
        Log("iot_smarthome_client_connect failed");
        return __FAILURE__;
    }

    // Subscribe the topics.
    iot_smarthome_client_dowork(handle);

    // Sample: get device shadow
    if (0 == iot_smarthome_client_get_shadow(handle, DEVICE, "123456789"))
    {
        Log("Succeeded to get device shadow");
    }
    else
    {
        Log("Failed to get device shadow");
    }

    // Sample: use 'serializer' to encode device model to binary and update the device shadow.
    BaiduSamplePump* pump = CREATE_MODEL_INSTANCE(BaiduIotDeviceSample, BaiduSamplePump);
    pump->FrequencyIn = 1;
    pump->FrequencyOut = 2;
    pump->Current = 3;
    pump->Speed = 4;
    pump->Torque = 5;
    pump->Power = 6;
    pump->DC_Bus_voltage = 7;
    pump->Output_voltage = 8;
    pump->Drive_temp = 9;

    unsigned char* reported;
    size_t reportedSize;
    if (CODEFIRST_OK != SERIALIZE(&reported, &reportedSize, pump->FrequencyIn, pump->FrequencyOut, pump->Current))
    {
        Log("Failed to serialize the reported binary");
    }
    else
    {
        char* reportedString = malloc(sizeof(char) * (reportedSize + 1));
        reportedString[reportedSize] = '\0';
        for (size_t index = 0; index < reportedSize; ++index)
        {
            reportedString[index] = reported[index];
        }

        if (0 == iot_smarthome_client_update_shadow_with_binary(handle, DEVICE, "123456", 0, reportedString, NULL))
        {
            Log("Succeeded to update device shadow with binary");
        }
        else
        {
            Log("Failed to update device shadow with binary");
        }

        // Sample: update shadow with incorrect version, and receive error message at 'update/rejected'.
        if (0 == iot_smarthome_client_update_shadow_with_binary(handle, DEVICE, "111111", 1, reportedString, NULL))
        {
            Log("Succeeded to send message for updating device shadow with binary");
        }
        else
        {
            Log("Failed to send message for updating device shadow with binary");
        }

        free(reportedString);
        free(reported);
    }

    DESTROY_MODEL_INSTANCE(pump);

    // Sample: subscribe the delta topic and update shadow with desired value.
    while (iot_smarthome_client_dowork(handle) >= 0)
    {
        ThreadAPI_Sleep(100);
    }

    iot_smarthome_client_deinit(handle);
    serializer_deinit();
    platform_deinit();

    return 0;

#ifdef _CRT_DBG_MAP_ALLOC
    _CrtDumpMemoryLeaks();
#endif
}

