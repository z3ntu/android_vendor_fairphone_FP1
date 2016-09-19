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

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	mknod.c

# The "mknod" command is needed for the init.*.rc files of the boot image (so it
# is built as a static executable and included in the boot image itself).
# However, the boot image files are copied to the recovery image, and as the
# "busybox" command built for the recovery image also contains a "mknod" command
# their names would clash and the build would fail. Therefore, even if this
# custom "mknod" command does not contain anything specific to the Fairphone 1,
# it is called "mknod-fp1" just to prevent the clash of names.
LOCAL_MODULE := mknod-fp1
LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)

LOCAL_STATIC_LIBRARIES := libc

LOCAL_FORCE_STATIC_EXECUTABLE := true

include $(BUILD_EXECUTABLE)
