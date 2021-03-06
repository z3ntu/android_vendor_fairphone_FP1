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

LOCAL_C_INCLUDES := \
    external/libnl-headers \
    external/wpa_supplicant_8/src \
    external/wpa_supplicant_8/src/drivers \
    external/wpa_supplicant_8/src/utils

LOCAL_SRC_FILES := driver_nl80211_cmd.c

LOCAL_MODULE := wpa_supplicant_8_private_lib_fp1

include $(BUILD_STATIC_LIBRARY)
