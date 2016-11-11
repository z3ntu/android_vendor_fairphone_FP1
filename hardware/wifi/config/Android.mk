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

ifeq ($(TARGET_DEVICE),fp1)



LOCAL_PATH:= $(call my-dir)



# Copy the android.conf file from external/dhcpcd as the final dhcpcd.conf.
include $(CLEAR_VARS)

LOCAL_SRC_FILES := ../../../../../../external/dhcpcd/android.conf

LOCAL_MODULE := dhcpcd.conf
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/dhcpcd

include $(BUILD_PREBUILT)



# Generate the wpa_supplicant.conf from the default template file.
ifeq ($(WPA_SUPPLICANT_VERSION),VER_0_8_X)
    WIFI_DRIVER_SOCKET_IFACE := wlan0

    include external/wpa_supplicant_8/wpa_supplicant/wpa_supplicant_conf.mk
endif



endif
