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


#include <iot_wechat_client.h>
#include "../../../serializer/inc/serializer.h"

// Prerequisite: your device should have string attribute audioUrl
//
//
// Please set the device client data and security which are shown as follow.
// The endpoint address your device should cnnect, which is like
// 1. "tcp://xxxxxx.mqtt.iot.xx.baidubce.com:1883" or
// 2. "ssl://xxxxxx.mqtt.iot.xx.baidubce.com:1884"
#define         ADDRESS             "tcp://e6e0474a54554682a1ad7711f2d1dad6.mqtt.iot.bj.baidubce.com:1883"

// The device name you created in device management service.
#define         DEVICE              "a_sample_device"

// The username you can find on the device connection configuration web,
// and the format is like "xxxxxx/xxxxx"
#define         USERNAME            "e6e0474a54554682a1ad7711f2d1dad6/a_sample_device"

// The key (password) you can find on the device connection configuration web.
#define         PASSWORD            "nWTZZUMP4qqDy38c1E96uJGy3ScjLygKrCQJHCxsjZ4="

static void playAudioByUrl(const char* url) {
    LogInfo("Please implement playing audio url here:");
    LogInfo(url);
}

int iot_wechat_client_run() {
    IOT_WECHAT_CLIENT_HANDLE handler = iot_wechat_client_init(ADDRESS, DEVICE, &playAudioByUrl);

    iot_wechat_client_subscribe(handler, DEVICE, USERNAME, PASSWORD);

    iot_wechat_client_deinit(handler);
    return 0;
}



