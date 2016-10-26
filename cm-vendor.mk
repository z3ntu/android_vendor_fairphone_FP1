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

ifeq ($(wildcard vendor/fairphone/fp1/proprietary/blobs.mk),)

$(warning "vendor/fairphone/fp1 is being used, but there are no proprietary \
           blobs. Did you forget to run extract-files.sh?")

else

# Copy the proprietary blobs.
$(call inherit-product, vendor/fairphone/fp1/proprietary/blobs.mk)

PRODUCT_COPY_FILES += \
	vendor/fairphone/fp1/rootdir/init.mt6589.proprietary.rc:root/init.mt6589.proprietary.rc

# Add "mknod-fp1" command, as it is needed by the init.mt6589.proprietary.rc
# file of the boot image.
PRODUCT_PACKAGES += \
	mknod-fp1

# Set the include directory for device-specific headers used to override or
# extend the standard headers.
TARGET_SPECIFIC_HEADER_PATH := vendor/fairphone/fp1/include/

# The proprietary graphics blobs support OpenGL ES 2.0.
PRODUCT_PROPERTY_OVERRIDES += \
	ro.opengles.version=131072

# Explicitly declare that a software implementation of OpenGL ES is not being
# used to prevent the FOSS device tree to enable it.
# This is not a standard property; the system must be patched to support it.
PRODUCT_PROPERTY_OVERRIDES += \
	ro.softwaregl=false

# By default, eglCreateWindowSurface (in
# "frameworks/native/opengl/libs/EGL/eglApi.cpp") picks the native window's
# buffers format from a specific set of formats instead of setting it to the
# EGL_NATIVE_VISUAL_ID format from the EGL config (as was done before Android
# 4.4). Unfortunately, this causes a mismatch between window and config pixel
# formats when using the proprietary OpenGL ES libraries from MediaTek, which in
# the end causes a failure in the graphics system. This issue seems to occur too
# with other proprietary OpenGL ES libraries, so the
# BOARD_EGL_WORKAROUND_BUG_10194508 build parameter can be enabled to revert the
# eglCreateWindowSurface behavior to the old one.
BOARD_EGL_WORKAROUND_BUG_10194508 := true

# CyanogenMod 11.0 changed GpsAidingData in
# "hardware/libhardware/include/hardware/gps.h" from uint16_t to uint32_t.
MTK_GPS_WRAPPER_GPS_HAL_USES_UINT32_AIDING_DATA := true

# Wrapper for the proprietary MediaTek GPS HAL module, needed to adjust its API
# to the standard GPS HAL module API.
PRODUCT_PACKAGES += \
	gps.fp1

# The file frameworks/native/data/etc/handheld_core_hardware.xml defines the
# minimum set of features that an Android-compatible device has to provide.
# Unfortunately, the FOSS device tree plus this proprietary vendor tree do not
# provide all the needed features, so instead of adding that file the specific
# files for each of the supported features are added (and, of course, also files
# for other extra features not included in handheld_core_hardware.xml, if any).
PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
	frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
	frameworks/native/data/etc/android.hardware.sensor.compass.xml:system/etc/permissions/android.hardware.sensor.compass.xml \
	frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
	frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
	frameworks/native/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml

endif
