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
#include <azure_c_shared_utility/platform.h>
#include <azure_c_shared_utility/threadapi.h>
#include "../../../serializer/inc/serializer.h"

// Prerequisite: your device should have string attribute audioUrl
//
//
// Please set the device client data and security which are shown as follow.
// The endpoint address your device should cnnect, which is like
// 1. "tcp://xxxxxx.mqtt.iot.xx.baidubce.com:1883" or
// 2. "ssl://xxxxxx.mqtt.iot.xx.baidubce.com:1884"
#define         SUB_ADDRESS             "tcp://e6e0474a54554682a1ad7711f2d1dad6.mqtt.iot.bj.baidubce.com:1883"

// The device name you created in device management service.
#define         DEVICE              "a_sample_device"

// The username you can find on the device connection configuration web,
// and the format is like "xxxxxx/xxxxx"
#define         SUB_USERNAME            "e6e0474a54554682a1ad7711f2d1dad6/a_sample_device"

// The key (password) you can find on the device connection configuration web.
#define         SUB_PASSWORD            "nWTZZUMP4qqDy38c1E96uJGy3ScjLygKrCQJHCxsjZ4="


// upload mqtt endpoint
// The endpoint address, witch is like "xxxxxx.mqtt.iot.xx.baidubce.com".
#define         PUB_ADDRESS                    "iot-wechat-demo.mqtt.iot.bj.baidubce.com"

// The mqtt client username, and the format is like "xxxxxx/xxxx".
#define         PUB_USERNAME                    "iot-wechat-demo/iot-wechat-device"

// The key (password) of mqtt client.
#define         PUB_PASSWORD                    "+biL5YtVCEPpZAhWfNCBwAaa8KZDc3u8TIm7I59dhok="

#define         PUB_TOPIC_NAME                  "iot-wechat-demo-topic1"

#define         VOICE_PIECE_SIZE_IN_BYTES       10000

static void playAudioByUrl(const char* url) {
    LogInfo("Please implement playing audio url here:");
    LogInfo(url);
}

int iot_wechat_client_run() {
    if (0 != platform_init())
    {
        LogError("platform_init failed");
        return __FAILURE__;
    }

    if (SERIALIZER_OK != serializer_init(NULL))
    {
        LogError("serializer_init failed");
        return __FAILURE__;
    }

    LogInfo("init iot wechat client start");

    IOT_WECHAT_CLIENT_HANDLE handler = iot_wechat_client_init(SUB_ADDRESS, SUB_USERNAME, SUB_PASSWORD, &playAudioByUrl, PUB_ADDRESS, PUB_USERNAME, PUB_PASSWORD, DEVICE);

    if (handler == NULL) {
        LogError("init client fail");
        return __FAILURE__;
    }
    LogInfo("init iot wechat client success");

    // read voice message from file and pub to mqtt
    FILE *fileptr = fopen("/Users/zhuzhu01/1519658635867.bytes", "rb");  // Open the file in binary mode
    fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
    long filelen = ftell(fileptr);             // Get the current byte offset in the file
    rewind(fileptr);                      // Jump back to the beginning of the file
    unsigned char* audio = (unsigned char *)malloc((filelen+1)*sizeof(char)); // Enough memory for file + \0
    fread(audio, (size_t) filelen, 1, fileptr); // Read in the entire file
    fclose(fileptr); // Close the file

    int result;
    // long voice message should pub in a sequence of mqtt messages
    // each mqtt message's payload mustn't exceed 32K
    int i = 0;
    int seq = 0;
    int finish = 0;
    char* messageId = "MyRandomMessageId";
    LogInfo("Voice length = %d", filelen);
    while (finish == 0) {
        int pieceLength = VOICE_PIECE_SIZE_IN_BYTES;
        if (i + VOICE_PIECE_SIZE_IN_BYTES > filelen) {
            finish = 1;
            pieceLength = filelen - i;
        }
        result = iot_wechat_client_pub_voice(handler, PUB_TOPIC_NAME, &audio[i], pieceLength, messageId, seq, 0);
        if (result != 0) {
            LogError("pub voice piece to mqtt fail");
            return __FAILURE__;
        }
        i += VOICE_PIECE_SIZE_IN_BYTES;
        seq++;
        LogInfo("publish voice piece to mqtt success, voicePieceLength=%d, seq=%d, isFinish=%d", pieceLength, seq, finish);
    }

    // pub a raw mqtt message
    result = iot_wechat_client_pub_data(handler, PUB_TOPIC_NAME, (unsigned char *)"hello from end", strlen("hello from end"));

    if (result != 0) {
        LogError("pub text message to mqtt fail");
        return __FAILURE__;
    }

    // subscribe should run in a dedicated thread
    iot_wechat_client_subscribe(handler);

    iot_wechat_client_deinit(handler);

    serializer_deinit();
    platform_deinit();

    return 0;
}



