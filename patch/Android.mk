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

.PHONY: patch-source-tree-for-fp1-vendor reverse-patch-source-tree-for-fp1-vendor abort-if-build-system-was-patched-vendor abort-if-build-system-was-reverse-patched-vendor

ifneq ($(filter fp1,$(TARGET_DEVICE)),)
    -include patch-source-tree-for-fp1-vendor
    -include abort-if-build-system-was-patched-vendor
else
    -include reverse-patch-source-tree-for-fp1-vendor
    -include abort-if-build-system-was-reverse-patched-vendor
endif

patch-source-tree-for-fp1-vendor:
	$(call patch-repository,system/core,vendor/fairphone/fp1/patch/add-nvram-user.patch)
	$(call patch-repository,hardware/libhardware_legacy,vendor/fairphone/fp1/patch/add-support-to-wifi-hal-to-add-and-remove-interfaces.patch,$(ABORT_IF_BUILD_SYSTEM_WAS_MODIFIED))

reverse-patch-source-tree-for-fp1-vendor:
	$(call reverse-patch-repository,system/core,vendor/fairphone/fp1/patch/add-nvram-user.patch)
	$(call reverse-patch-repository,hardware/libhardware_legacy,vendor/fairphone/fp1/patch/add-support-to-wifi-hal-to-add-and-remove-interfaces.patch,$(ABORT_IF_BUILD_SYSTEM_WAS_MODIFIED))

abort-if-build-system-was-patched-vendor: | patch-source-tree-for-fp1-vendor
	$(if $(wildcard $(ABORT_IF_BUILD_SYSTEM_WAS_MODIFIED)), \
	    $(shell rm -f $(ABORT_IF_BUILD_SYSTEM_WAS_MODIFIED)) \
	    $(error The build system was patched. Launch again the build))

abort-if-build-system-was-reverse-patched-vendor: | reverse-patch-source-tree-for-fp1-vendor
	$(if $(wildcard $(ABORT_IF_BUILD_SYSTEM_WAS_MODIFIED)), \
	    $(shell rm -f $(ABORT_IF_BUILD_SYSTEM_WAS_MODIFIED)) \
	    $(error The build system was reverse patched. Launch again the build))
