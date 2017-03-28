#
# Copyright (C) 2016-2017 The Android Open Source Project
# Copyright 2017 NXP
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS := \
	$(LOCAL_DIR)/manifest.c \
	$(LOCAL_DIR)/main.c \
	$(LOCAL_DIR)/hwrng_srv.c \
	$(LOCAL_DIR)/hwkey_srv.c \
	$(LOCAL_DIR)/hwkey_srv_provider.c \
	$(LOCAL_DIR)/hwrng_srv_provider.c \

MODULE_DEPS := \
	app/trusty \
	lib/libc-trusty \
	interface/hwrng \
	interface/hwkey \
	openssl \

include make/module.mk