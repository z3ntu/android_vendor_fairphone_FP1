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

ALL_FILES=""



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

    # If a parameter is given it is assumed to be a root directory to copy the
    # files from; otherwise, the files are pulled from the device using ADB.
    if [ "$#" -eq "1" ]; then
        cp "$1/$FILE" "$VENDOR_DIR/proprietary/$FILE"
    else
        adb pull "$FILE" "$VENDOR_DIR/proprietary/$FILE"
    fi

    echo -n " \\"$'\n'"	vendor/fairphone/fp1/proprietary/$FILE:$FILE" >> $MK_FILE
done

echo "" >> $MK_FILE
