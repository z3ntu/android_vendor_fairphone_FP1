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

# vendor tree counterpart for device/fairphone/fp1/patch/Android.mk

.PHONY: patch-source-tree-for-fp1-vendor reverse-patch-source-tree-for-fp1-vendor

ifneq ($(filter fp1,$(TARGET_DEVICE)),)
    -include patch-source-tree-for-fp1-vendor
else
    -include reverse-patch-source-tree-for-fp1-vendor
endif

patch-source-tree-for-fp1-vendor:
	$(call patch-repository,system/core,vendor/fairphone/fp1/patch/add-xlog-buf-printf.patch)
	$(call patch-repository,system/core,vendor/fairphone/fp1/patch/add-nvram-user.patch)

reverse-patch-source-tree-for-fp1-vendor:
	$(call reverse-patch-repository,system/core,vendor/fairphone/fp1/patch/add-xlog-buf-printf.patch)
	$(call reverse-patch-repository,system/core,vendor/fairphone/fp1/patch/add-nvram-user.patch)
