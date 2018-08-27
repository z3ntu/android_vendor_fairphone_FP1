#!/bin/bash
#
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

# Proprietary blob extractor.
#
# If no parameter is given, this script copies the proprietary blobs by pulling
# them from the connected device using ADB (only one device is expected to be
# connected when this script is run).
#
# If a parameter is given, this script copies the proprietary blobs from the
# subdirectories of the given parameter (it is assumed that the parameter is the
# path to the root directory of an uncompressed ROM).

VENDOR_DIR="."

# If realpath is available use it to find the absolute path to the vendor dir
# (just in case this script was not invoked from vendor/fairphone/fp1)
if command -v realpath >/dev/null; then
    VENDOR_DIR="$(realpath --canonicalize-missing $(dirname $0)/$VENDOR_DIR)"
fi



# Files to be extracted

libpvr2d_so_DEPENDENCIES=" \
    system/vendor/lib/libsrv_um.so"

gralloc_mt6589_so_DEPENDENCIES=" \
    system/vendor/lib/libsrv_um.so \
    $libpvr2d_so_DEPENDENCIES \
    system/vendor/lib/libpvr2d.so \
    system/lib/libion.so"

GRALLOC_HAL=" \
    $gralloc_mt6589_so_DEPENDENCIES \
    system/vendor/lib/hw/gralloc.mt6589.so"



libsrv_init_so_DEPENDENCIES=" \
    system/vendor/lib/libsrv_um.so"

pvrsrvctl_DEPENDENCIES=" \
    $libsrv_init_so_DEPENDENCIES \
    system/vendor/lib/libsrv_init.so"

GRAPHICS_SETUP=" \
    $pvrsrvctl_DEPENDENCIES \
    system/vendor/bin/pvrsrvctl"



libIMGegl_so_DEPENDENCIES=" \
    system/vendor/lib/libsrv_um.so"

libEGL_mtk_so_DEPENDENCIES=" \
    $libIMGegl_so_DEPENDENCIES \
    system/vendor/lib/libIMGegl.so"

libGLESv1_CM_mtk_so_DEPENDENCIES=" \
    system/vendor/lib/libsrv_um.so \
    $libIMGegl_so_DEPENDENCIES \
    system/vendor/lib/libIMGegl.so \
    system/vendor/lib/libusc.so"

libGLESv2_mtk_so_DEPENDENCIES=" \
    system/vendor/lib/libsrv_um.so \
    $libIMGegl_so_DEPENDENCIES \
    system/vendor/lib/libIMGegl.so"

GRAPHICS_OPENGL=" \
    $libEGL_mtk_so_DEPENDENCIES \
    system/vendor/lib/egl/libEGL_mtk.so \
    $libGLESv1_CM_mtk_so_DEPENDENCIES \
    system/vendor/lib/egl/libGLESv1_CM_mtk.so \
    $libGLESv2_mtk_so_DEPENDENCIES \
    system/vendor/lib/egl/libGLESv2_mtk.so"



libglslcompiler_so_DEPENDENCIES=" \
    system/vendor/lib/libsrv_um.so \
    system/vendor/lib/libusc.so"

libpvrANDROID_WSEGL_so_DEPENDENCIES=" \
    system/vendor/lib/libsrv_um.so"

libPVRScopeServices_so_DEPENDENCIES=" \
    system/vendor/lib/libsrv_um.so"

GRAPHICS_OTHER=" \
    $libglslcompiler_so_DEPENDENCIES \
    system/vendor/lib/libglslcompiler.so \
    $libpvr2d_so_DEPENDENCIES \
    system/vendor/lib/libpvr2d.so \
    $libpvrANDROID_WSEGL_so_DEPENDENCIES \
    system/vendor/lib/libpvrANDROID_WSEGL.so \
    system/vendor/lib/libusc.so \
    $libPVRScopeServices_so_DEPENDENCIES \
    system/vendor/lib/libPVRScopeServices.so"



GRAPHICS=" \
    $GRALLOC_HAL
    $GRAPHICS_SETUP \
    $GRAPHICS_OPENGL \
    $GRAPHICS_OTHER"






libnvram_so_DEPENDENCIES=" \
    system/lib/libcustom_nvram.so"

libfile_op_so_DEPENDENCIES=" \
    $libnvram_so_DEPENDENCIES \
    system/lib/libnvram.so \
    system/lib/libcustom_nvram.so"

libhwm_so_DEPENDENCIES=" \
    $libnvram_so_DEPENDENCIES \
    system/lib/libnvram.so \
    $libfile_op_so_DEPENDENCIES \
    system/lib/libfile_op.so"

libnvram_daemon_callback_so_DEPENDENCIES=" \
    system/lib/libcustom_nvram.so \
    $libnvram_so_DEPENDENCIES \
    system/lib/libnvram.so"

nvram_daemon_DEPENDENCIES=" \
    $libnvram_so_DEPENDENCIES \
    system/lib/libnvram.so \
    system/lib/libcustom_nvram.so \
    $libfile_op_so_DEPENDENCIES \
    system/lib/libfile_op.so \
    $libhwm_so_DEPENDENCIES \
    system/lib/libhwm.so \
    $libnvram_daemon_callback_so_DEPENDENCIES \
    system/lib/libnvram_daemon_callback.so"

NVRAM=" \
    $nvram_daemon_DEPENDENCIES \
    system/bin/nvram_daemon"






SENSORS_HAL=" \
    system/lib/hw/sensors.default.so"



SENSORS_MAGNETOMETER_DAEMON=" \
    system/bin/memsicd3416x"



SENSORS=" \
    $SENSORS_HAL \
    $SENSORS_MAGNETOMETER_DAEMON"






WIRELESS_COMBO_CHIP_FIRMWARE=" \
    system/etc/firmware/mt6628_ant_m1.cfg \
    system/etc/firmware/mt6628_patch_e1_hdr.bin \
    system/etc/firmware/mt6628_patch_e2_0_hdr.bin \
    system/etc/firmware/mt6628_patch_e2_1_hdr.bin \
    system/etc/firmware/WIFI_RAM_CODE_MT6628 \
    system/etc/firmware/WMT.cfg"



WIRELESS_COMBO_CHIP_LAUNCHER=" \
    system/bin/6620_launcher"



WIRELESS_COMBO_CHIP=" \
    $WIRELESS_COMBO_CHIP_FIRMWARE \
    $WIRELESS_COMBO_CHIP_LAUNCHER"






GPS_HAL=" \
    system/lib/hw/gps.default.so"



libnvram_so_DEPENDENCIES=" \
    system/lib/libcustom_nvram.so"

libmnlp_mt6628_DEPENDENCIES=" \
    $libnvram_so_DEPENDENCIES \
    system/lib/libnvram.so"

# mnld is a daemon used by "system/lib/hw/gps.default.so". It is not a hardcoded
# dependency, but launched through an init service.
# libmnlp_mt6628 is a daemon used by "system/xbin/mnld". It is not a hardcoded
# dependency, but selected and launched dynamically based on the actual chipset.
GPS_DAEMON=" \
    system/xbin/mnld \
    $libmnlp_mt6628_DEPENDENCIES \
    system/xbin/libmnlp_mt6628"



GPS=" \
    $GPS_HAL \
    $GPS_DAEMON"






AUDIO_POLICY_DEPENDENCIES=" \
    system/lib/libaed.so"

LIBBLISRC_DEPENDENCIES=" \
    system/lib/libmtk_drvb.so"

AUDIO_PRIMARY_DEPENDENCIES=" \
    $LIBBLISRC_DEPENDENCIES \
    system/lib/libblisrc.so \
    system/lib/libspeech_enh_lib.so"

LIBAUDIOCOMPENSATIONFILTER_DEPENDENCIES=" \
    system/lib/libbessound_mtk.so"

AUDIO=" \
    $AUDIO_POLICY_DEPENDENCIES \
    system/lib/hw/audio_policy.default.so:system/lib/hw/audio_policy.mt6589.so \
    $AUDIO_PRIMARY_DEPENDENCIES \
    system/lib/libaudio.primary.default.so:system/lib/hw/audio.primary.mt6589.so \
    system/lib/libaudiocustparam.so \
    system/lib/libaudiosetting.so \
    $LIBAUDIOCOMPENSATIONFILTER_DEPENDENCIES \
    system/lib/libaudiocompensationfilter.so \
    system/etc/audio_policy.conf \
    system/etc/audio_effects.conf"






ALL_FILES=" \
    $GRAPHICS \
    $NVRAM \
    $SENSORS \
    $WIRELESS_COMBO_CHIP \
    $GPS \
    $AUDIO"



# Remove duplicated files

FILES=""
for FILE in $ALL_FILES; do
    if [ -z "$(echo "$FILES" | grep --word-regexp "$FILE")" ]; then
        FILES="$FILES $FILE"
    fi
done



# Perform the extraction and create a blobs.mk file to install them

MK_FILE="$VENDOR_DIR/proprietary/blobs.mk"

mkdir --parents "$(dirname $MK_FILE)"
rm -f $MK_FILE
echo -n "PRODUCT_COPY_FILES +=" >> $MK_FILE

for FILE in $FILES; do
    mkdir --parents "$(dirname $VENDOR_DIR/proprietary/$FILE)"

    # Possibility to rename files ("SRCFILE:DSTFILE")
    if [[ $FILE = *":"* ]]; then
        FILEARR=(${FILE//:/ })
        FILE_SRC=${FILEARR[0]}
        FILE_DST=${FILEARR[1]}
    else
        FILE_SRC=$FILE
        FILE_DST=$FILE
    fi

    # If a parameter is given it is assumed to be a root directory to copy the
    # files from; otherwise, the files are pulled from the device using ADB.
    if [ "$#" -eq "1" ]; then
        cp "$1/$FILE_SRC" "$VENDOR_DIR/proprietary/$FILE_DST"
    else
        adb pull "$FILE_SRC" "$VENDOR_DIR/proprietary/$FILE_DST"
    fi

    echo -n " \\"$'\n'"	vendor/fairphone/fp1/proprietary/$FILE_DST:$FILE_DST" >> $MK_FILE
done

echo "" >> $MK_FILE
