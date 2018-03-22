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

/* This file contains cert trust chain needed to communicate with Baidu (IoT Hub) */

#include "certs.h"

const char certificates[] =
        // add trust external ca root
        "-----BEGIN CERTIFICATE-----\n"
                "MIIECzCCAvOgAwIBAgIIY2iFs2sfg4AwDQYJKoZIhvcNAQELBQAwMzEWMBQGA1UE\n"
                "AwwNaW90LmJhaWR1LmNvbTEMMAoGA1UECwwDQkNFMQswCQYDVQQGEwJDTjAeFw0x\n"
                "ODAzMTYwMjU2NDRaFw0zODAzMTEwMjU2NDRaMDMxFjAUBgNVBAMMDWlvdC5iYWlk\n"
                "dS5jb20xDDAKBgNVBAsMA0JDRTELMAkGA1UEBhMCQ04wggEiMA0GCSqGSIb3DQEB\n"
                "AQUAA4IBDwAwggEKAoIBAQC5nhJm26tqd4Mr1jGIPPz4wPSV3JVfYtsU2Bc8Wjkq\n"
                "pI01LisRpzZ9YP0/Hr8Ow0wZcREBP8HKYE4zhuwYy3modJyv/oNEFMVcoBcG7zpR\n"
                "aDdRZlYC63xdyGU1Nnr/BDDSQOhC62cDxYpd3gR+HJ1oDXy2P/6quy0fVButmMy2\n"
                "GLCo19e5g+vHp0Syzr97DS2MJ87EbKrQGzMOcXFRHC9IPBvZ7tR1pQeifWOazLRs\n"
                "wumyWuFYMk1OP6DqX/JZrzLI42n3Mk9f9CPA7evocHXOfYrllkw0Hgf7lQy8HJiG\n"
                "BSWkMKaU/VV18GRLiu2uLECq5bqOY7BDHJlKnkitjYz/AgMBAAGjggEhMIIBHTAd\n"
                "BgNVHQ4EFgQU3QjkFZ/W/PwBa/uNSrfV2wVwj2MwDwYDVR0TAQH/BAUwAwEB/zAf\n"
                "BgNVHSMEGDAWgBTdCOQVn9b8/AFr+41Kt9XbBXCPYzCBuQYDVR0gBIGxMIGuMIGr\n"
                "BgNVHR8wgaMwgaAGCCsGAQUFBwIBFoGTaHR0cDovL3BraS5pb3YuYmFpZHViY2Uu\n"
                "Y29tL3YxL3BraS9jcmw/Y21kPWNybCZmb3JtYXQ9UEVNJmlzc3Vlcj1DJTNEQ04l\n"
                "MkNDTiUzRGlvdC5iYWlkdS5jb20lMkNVJTNEQmFpZHUlMkNPVSUzREJDRSUyQ01B\n"
                "SUwlM0RiY2VfaW90X3JkJTQwYmFpZHUuY29tMA4GA1UdDwEB/wQEAwIBhjANBgkq\n"
                "hkiG9w0BAQsFAAOCAQEAb4OO+msyE6rl/gknrGu81P8Ff46K87j5+B8TA1Q1DUOR\n"
                "7X5vct+S5Zl4cN102V93VjC32IcQc4UlGVckfHOufO0XuqsJeLQBSgBNKDSAJmVv\n"
                "zcoKGzDa/z+KA1kUT8jB/8g7VSjudE1DF3V8YACFJ5BEDlEumaNqBKXyGLlsp8qn\n"
                "9jpF6lkkr2wQCgI18NO5BhZQxJdFLnOGRZxZdXMLHm8cpBX6SdVrPf4vXjzP180s\n"
                "/9PMKl5zUudRyC6dvmBgyVc7SFK2/1necA/xVVERG9wryrVmkEmuMcpWEcYGDiHZ\n"
                "YN4pIlAwNM2W/sTng25TsQJTAI/RY6A8o73cddr+Qw==\n"
                "-----END CERTIFICATE-----"
;

// add your client cert of the principal
const char client_cert[] =
"-----BEGIN CERTIFICATE-----\n"
        "your client cert\n"
        "-----END CERTIFICATE-----"
;

// add your client private key of the principal
const char client_key[] =
"-----BEGIN RSA PRIVATE KEY-----\n"
        "your client key\n"
        "-----END RSA PRIVATE KEY-----"
;

const char bos_root_ca[] = "-----BEGIN CERTIFICATE-----\n"
    "MIIE0zCCA7ugAwIBAgIQGNrRniZ96LtKIVjNzGs7SjANBgkqhkiG9w0BAQUFADCB\n"
    "yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL\n"
    "ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJp\n"
    "U2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxW\n"
    "ZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0\n"
    "aG9yaXR5IC0gRzUwHhcNMDYxMTA4MDAwMDAwWhcNMzYwNzE2MjM1OTU5WjCByjEL\n"
    "MAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZW\n"
    "ZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJpU2ln\n"
    "biwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxWZXJp\n"
    "U2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0aG9y\n"
    "aXR5IC0gRzUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCvJAgIKXo1\n"
    "nmAMqudLO07cfLw8RRy7K+D+KQL5VwijZIUVJ/XxrcgxiV0i6CqqpkKzj/i5Vbex\n"
    "t0uz/o9+B1fs70PbZmIVYc9gDaTY3vjgw2IIPVQT60nKWVSFJuUrjxuf6/WhkcIz\n"
    "SdhDY2pSS9KP6HBRTdGJaXvHcPaz3BJ023tdS1bTlr8Vd6Gw9KIl8q8ckmcY5fQG\n"
    "BO+QueQA5N06tRn/Arr0PO7gi+s3i+z016zy9vA9r911kTMZHRxAy3QkGSGT2RT+\n"
    "rCpSx4/VBEnkjWNHiDxpg8v+R70rfk/Fla4OndTRQ8Bnc+MUCH7lP59zuDMKz10/\n"
    "NIeWiu5T6CUVAgMBAAGjgbIwga8wDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8E\n"
    "BAMCAQYwbQYIKwYBBQUHAQwEYTBfoV2gWzBZMFcwVRYJaW1hZ2UvZ2lmMCEwHzAH\n"
    "BgUrDgMCGgQUj+XTGoasjY5rw8+AatRIGCx7GS4wJRYjaHR0cDovL2xvZ28udmVy\n"
    "aXNpZ24uY29tL3ZzbG9nby5naWYwHQYDVR0OBBYEFH/TZafC3ey78DAJ80M5+gKv\n"
    "MzEzMA0GCSqGSIb3DQEBBQUAA4IBAQCTJEowX2LP2BqYLz3q3JktvXf2pXkiOOzE\n"
    "p6B4Eq1iDkVwZMXnl2YtmAl+X6/WzChl8gGqCBpH3vn5fJJaCGkgDdk+bW48DW7Y\n"
    "5gaRQBi5+MHt39tBquCWIMnNZBU4gcmU7qKEKQsTb47bDN0lAtukixlE0kF6BWlK\n"
    "WE9gyn6CagsCqiUXObXbf+eEZSqVir2G3l6BFoMtEMze/aiCKm0oHw0LxOXnGiYZ\n"
    "4fQRbxC1lfznQgUy286dUV4otp6F01vvpX1FQHKOtw5rDgb7MzVIcbidJ4vEZV8N\n"
    "hnacRHr2lVz2XTIIM6RUthg/aFzyQkqFOFSDX9HoLPKsEdao7WNq\n"
    "-----END CERTIFICATE-----";