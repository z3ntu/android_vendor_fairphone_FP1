# Copyright (C) 2016 Daniel Calviño Sánchez <danxuliu@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := gps.c

LOCAL_SHARED_LIBRARIES := libcutils libdl liblog

LOCAL_MODULE := gps.fp1
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw

ifneq ($(MTK_GPS_WRAPPER_WRAPPED_MODULE_PATH),)
    LOCAL_CFLAGS += -DWRAPPED_MODULE_PATH=\"$(MTK_GPS_WRAPPER_WRAPPED_MODULE_PATH)\"
else
    LOCAL_CFLAGS += -DWRAPPED_MODULE_PATH=\"/system/lib/hw/gps.default.so\"
endif

ifneq ($(MTK_GPS_WRAPPER_GPS_HAL_USES_UINT32_AIDING_DATA),)
    LOCAL_CFLAGS += -DGPS_HAL_USES_UINT32_AIDING_DATA
endif

include $(BUILD_SHARED_LIBRARY)
