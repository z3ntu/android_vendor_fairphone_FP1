/*
 * Copyright (C) 2016 Daniel Calviño Sánchez <danxuliu@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define LOG_TAG "gps.fp1"

#include <dlfcn.h>
#include <errno.h>

#include <cutils/log.h>

#include <hardware/gps.h>

/**
 * Wrapper for proprietary MediaTek GPS HAL module.
 *
 * The proprietary GPS HAL module from MediaTek was built against a non-standard
 * "hardware/gps.h" header. Therefore, when used in a system with a standard
 * header it does not work as expected. This GPS HAL module wraps the MediaTek
 * one and acts as an interface between it and a standard system.
 *
 * The "hardware/gps.h" header that the propietary MediaTek GPS HAL module was
 * built against is unknown. It was found that there are changes at least in the
 * GpsCallbacks and GpsSvStatus structs: in GpsCallbacks there is an extra
 * callback somewhere between nmea_cb and create_thread_cb, and the GpsSvStatus
 * supports up to 256 SVs instead of just 32.
 *
 * Due to those changes, this wrapper must adapt the GpsCallbacks passed from
 * the client to the wrapped module when the GpsInterface is inited and also
 * adapt the GpsSvStatus passed from the wrapped module to the client when the
 * sv_status callback is called.
 *
 * To do that, this module opens the proprietary module and just returns the
 * functions and data structures provided by it, except when the behavior needs
 * tweaking. In those cases, it provides its own implementation, but keeps a
 * reference to the original functions and data structures in order to call them
 * if needed with the modified parameters. Depending on the case, that reference
 * is kept either in an additional field in the same data structure exposed to
 * the client (when the functions receive the data structure that they belong to
 * as a parameter) or in a variable internal to this module (when the functions
 * do not receive the data structure that they belong to as a parameter).
 *
 * Due to that later case, it is a must that just a single GpsInterface and a
 * single set of GpsCallbacks are used at a time. If several GpsInterfaces were
 * got or a GpsInterface was inited with different GpsCallbacks and the old ones
 * were used along with the latest ones this GPS HAL module wrapper would fail.
 * However, as the GPS HAL module API does not make possible to know from which
 * GPS device is a GpsInterface got or in which GpsInterface are the callbacks
 * inited, it is assumed that a single GpsInterface and a single set of
 * GpsCallbacks will be used at a time.
 *
 * In any case, all the definitions are based on
 * "hardware/libhardware/include/hardware/gps.h" from AOSP 4.2, commit
 * ee43a308b6. The proprietary MediaTek GPS HAL module to be wrapped must be the
 * one from the official Fairphone 1 release for Android 4.2; other proprietary
 * MediaTek GPS HAL modules may not work as intended.
 */

#define MEDIATEK_GPS_MAX_SVS 256
#define MEDIATEK_GPS_MASK_COUNT (MEDIATEK_GPS_MAX_SVS / 32)

/**
 * MediaTek SV status.
 *
 * The standard GpsSvStatus supports up to GPS_MAX_SVS (32) SVs, while the
 * MediaTek GpsSvStatus version seems to support up to 256.
 *
 * As the standard GpsSvStatus has 32 SVs, all the SVs fit in uint32 masks.
 * However, with the increased amount of SVs in the MediaTek version, each mask
 * needs now 256 bits, so arrays of eight uint32 are used instead.
 *
 * Note, however, that nothing guarantees that this layout is indeed the layout
 * used in the proprietary MediaTek GPS HAL module. It is just hypothetical
 * (although it seems to work).
 */
struct mediatek_gps_sv_status {
    size_t      size;
    int         num_svs;
    GpsSvInfo   sv_list[MEDIATEK_GPS_MAX_SVS];
    uint32_t    ephemeris_mask[MEDIATEK_GPS_MASK_COUNT];
    uint32_t    almanac_mask[MEDIATEK_GPS_MASK_COUNT];
    uint32_t    used_in_fix_mask[MEDIATEK_GPS_MASK_COUNT];
};

/**
 * The parameters passed to the GpsCallbacks functions do not include the
 * GpsCallbacks structure that they belong to. Therefore, when the callbacks are
 * set it is necessary to keep a pointer to the wrapped GpsCallbacks in order to
 * call them from the wrappers.
 *
 * Note that, due to this, if several different callbacks were set this GPS HAL
 * module wrapper would fail if the old callbacks were used again.
 */
static GpsCallbacks* current_wrapped_gps_callbacks = 0;

static void sv_status_callback(struct mediatek_gps_sv_status* mediatek_sv_status) {
    ALOGV("Calling sv_status_callback wrapper");

    // The GpsSvStatus is not expected to be used outside the wrapped callback,
    // so just create it in the stack.
    GpsSvStatus standard_sv_status;
    standard_sv_status.size = sizeof(GpsSvStatus);
    standard_sv_status.num_svs = mediatek_sv_status->num_svs;
    memcpy(standard_sv_status.sv_list, mediatek_sv_status->sv_list, sizeof(standard_sv_status.sv_list));
    standard_sv_status.ephemeris_mask = mediatek_sv_status->ephemeris_mask[0];
    standard_sv_status.almanac_mask = mediatek_sv_status->almanac_mask[0];
    standard_sv_status.used_in_fix_mask = mediatek_sv_status->used_in_fix_mask[0];

    current_wrapped_gps_callbacks->sv_status_cb(&standard_sv_status);
}

static void unknown_padding_callback_stub(uint32_t capabilities) {
    ALOGW("TODO: stub for unknown_padding_callback; ensure that this is the expected callback and disable it in gps_interface_init or fix the mediatek_gps_callbacks");
}

static void set_capabilities_callback_stub(uint32_t capabilities) {
    ALOGW("TODO: stub for set_capabilities_callback; ensure that this is the expected callback and enable it in gps_interface_init or fix the mediatek_gps_callbacks");
}

static void acquire_wakelock_callback_stub() {
    ALOGW("TODO: stub for acquire_wakelock_callback; ensure that this is the expected callback and enable it in gps_interface_init or fix the mediatek_gps_callbacks");
}

static void release_wakelock_callback_stub() {
    ALOGW("TODO: stub for release_wakelock_callback; ensure that this is the expected callback and enable it in gps_interface_init or fix the mediatek_gps_callbacks");
}

/**
 * MediaTek GPS callback.
 *
 * The create_thread callback in the MediaTek GpsCallback version is at the
 * same location as the request_utc_time callback in the standard version. The
 * nmea callback and those preceding it are at the same location in both
 * versions. Therefore, there is an unknown callback between them. However, the
 * exact location is also unknown, as the set_capabilities, acquire_wakelock and
 * release_wakelock do not seem to be ever called, so they can not be used as a
 * reference.
 */
static struct mediatek_gps_callbacks {
    size_t      size;
    gps_location_callback location_cb;
    gps_status_callback status_cb;
    void (*sv_status_cb)(struct mediatek_gps_sv_status* mediatek_sv_status);
    gps_nmea_callback nmea_cb;
    void (*unknown_padding_cb)();
    gps_set_capabilities set_capabilities_cb;
    gps_acquire_wakelock acquire_wakelock_cb;
    gps_release_wakelock release_wakelock_cb;
    gps_create_thread create_thread_cb;
    gps_request_utc_time request_utc_time_cb;
} current_mediatek_gps_callbacks;

struct mediatek_gps_interface {
    size_t size;
    int (*init)(struct mediatek_gps_callbacks* callbacks);
    int (*start)(void);
    int (*stop)(void);
    void (*cleanup)(void);
    int (*inject_time)(GpsUtcTime time, int64_t timeReference, int uncertainty);
    int (*inject_location)(double latitude, double longitude, float accuracy);
    void (*delete_aiding_data)(GpsAidingData flags);
    int (*set_position_mode)(GpsPositionMode mode, GpsPositionRecurrence recurrence,
         uint32_t min_interval, uint32_t preferred_accuracy, uint32_t preferred_time);
    const void* (*get_extension)(const char* name);
};

struct gps_interface_wrapper {
    GpsInterface gps_interface;

    const struct mediatek_gps_interface* wrapped_gps_interface;
};

/**
 * The parameters passed to the GpsInterface functions do not include the
 * GpsInterface structure that they belong to. Therefore, when the interface is
 * got it is necessary to keep a pointer to the wrapped GpsInterface in order to
 * call the functions in the wrapped one from the wrappers.
 *
 * Note that, due to this, if several different GpsInterfaces were got this GPS
 * HAL module wrapper would fail if the old GpsInterfaces were used again.
 */
static struct gps_interface_wrapper* current_gps_interface_wrapper = 0;

static int gps_interface_init(GpsCallbacks* callbacks) {
    ALOGV("Initing wrapped GPS interface");

    if (current_wrapped_gps_callbacks) {
        ALOGW("gps_interface_init called again; old current_wrapped_gps_callbacks is now invalid!");
    }
    current_wrapped_gps_callbacks = callbacks;

    current_mediatek_gps_callbacks.size = sizeof(current_mediatek_gps_callbacks);
    current_mediatek_gps_callbacks.location_cb = callbacks->location_cb;
    current_mediatek_gps_callbacks.status_cb = callbacks->status_cb;

    current_mediatek_gps_callbacks.sv_status_cb = &sv_status_callback;

    current_mediatek_gps_callbacks.nmea_cb = callbacks->nmea_cb;

    // These callbacks are not guaranteed to be in the right order in the
    // MediaTek GpsCallbacks, so for the time being just log that they were
    // called to try to figure that right order.
    current_mediatek_gps_callbacks.unknown_padding_cb = &unknown_padding_callback_stub;
    current_mediatek_gps_callbacks.set_capabilities_cb = &set_capabilities_callback_stub;
    current_mediatek_gps_callbacks.acquire_wakelock_cb = &acquire_wakelock_callback_stub;
    current_mediatek_gps_callbacks.release_wakelock_cb = &release_wakelock_callback_stub;

    current_mediatek_gps_callbacks.create_thread_cb = callbacks->create_thread_cb;
    current_mediatek_gps_callbacks.request_utc_time_cb = callbacks->request_utc_time_cb;

    return current_gps_interface_wrapper->wrapped_gps_interface->init(&current_mediatek_gps_callbacks);
}

struct mediatek_gps_device_t {
    struct hw_device_t common;

    const struct mediatek_gps_interface* (*get_gps_interface)(struct mediatek_gps_device_t* dev);
};

struct gps_device_wrapper {
    struct gps_device_t device;

    struct mediatek_gps_device_t* wrapped_device;
};

static const GpsInterface* gps_device_get_gps_interface(struct gps_device_t* device) {
    ALOGV("Getting GPS interface wrapper");

    struct gps_device_wrapper* wrapper = (struct gps_device_wrapper*) device;

    const struct mediatek_gps_interface* wrapped_gps_interface = wrapper->wrapped_device->get_gps_interface(wrapper->wrapped_device);

    struct gps_interface_wrapper* interface_wrapper = malloc(sizeof(struct gps_interface_wrapper));
    memset(interface_wrapper, 0, sizeof(struct gps_interface_wrapper));

    // Only the init function has to be overriden in the GPS interface.
    interface_wrapper->gps_interface.size = sizeof(struct mediatek_gps_interface);
    interface_wrapper->gps_interface.init = &gps_interface_init;
    interface_wrapper->gps_interface.start = wrapped_gps_interface->start;
    interface_wrapper->gps_interface.stop = wrapped_gps_interface->stop;
    interface_wrapper->gps_interface.cleanup = wrapped_gps_interface->cleanup;
    interface_wrapper->gps_interface.inject_time = wrapped_gps_interface->inject_time;
    interface_wrapper->gps_interface.inject_location = wrapped_gps_interface->inject_location;
    interface_wrapper->gps_interface.delete_aiding_data = wrapped_gps_interface->delete_aiding_data;
    interface_wrapper->gps_interface.set_position_mode = wrapped_gps_interface->set_position_mode;
    interface_wrapper->gps_interface.get_extension = wrapped_gps_interface->get_extension;
    interface_wrapper->wrapped_gps_interface = wrapped_gps_interface;

    if (current_gps_interface_wrapper) {
        ALOGW("get_gps_interface called again; old current_gps_interface_wrapper is now invalid!");

        free(current_gps_interface_wrapper);
    }
    current_gps_interface_wrapper = interface_wrapper;

    return (GpsInterface*) current_gps_interface_wrapper;
}

static int gps_device_close(struct hw_device_t* device) {
    ALOGV("Closing GPS device wrapper");

    struct gps_device_wrapper* wrapper = (struct gps_device_wrapper*)device;

    int result = wrapper->wrapped_device->common.close(&wrapper->wrapped_device->common);

    free(device);

    return result;
}

static int gps_module_open(const struct hw_module_t* module, const char* name, struct hw_device_t** device) {
    ALOGI("Opening MediaTek GPS wrapper HAL module for '%s'", WRAPPED_MODULE_PATH);

    void* wrapped_module_handle = dlopen(WRAPPED_MODULE_PATH, RTLD_NOW);
    if (!wrapped_module_handle) {
        ALOGE("Could not dlopen the wrapped MediaTek GPS module: %s", dlerror());

        return -EINVAL;
    }

    // Reset errors before calling dlsym for proper error checking.
    dlerror();

    struct hw_module_t* wrapped_module = (struct hw_module_t*) dlsym(wrapped_module_handle, HAL_MODULE_INFO_SYM_AS_STR);

    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        ALOGE("Could not find the HAL module symbol in the wrapped MediaTek GPS module: %s", dlsym_error);

        return -EINVAL;
    }

    if (strcmp(wrapped_module->id, GPS_HARDWARE_MODULE_ID) != 0) {
        ALOGE("Invalid wrapped MediaTek GPS module; expected id is '%s', but '%s' was found", GPS_HARDWARE_MODULE_ID, wrapped_module->id);

        dlclose(wrapped_module_handle);

        return -EINVAL;
    }

    struct gps_device_wrapper* wrapper = (struct gps_device_wrapper*) malloc(sizeof(struct gps_device_wrapper));
    memset(wrapper, 0, sizeof(struct gps_device_wrapper));

    wrapper->device.common.tag = HARDWARE_DEVICE_TAG;
    wrapper->device.common.version = HARDWARE_DEVICE_API_VERSION(1, 0);
    wrapper->device.common.module = (struct hw_module_t*) module;
    wrapper->device.common.close = &gps_device_close;
    wrapper->device.get_gps_interface = &gps_device_get_gps_interface;

    struct mediatek_gps_device_t* wrapped_device;
    int wrapped_status = wrapped_module->methods->open(wrapped_module, name, (struct hw_device_t**) &wrapped_device);
    if (wrapped_status != 0) {
        ALOGE("Failed to open wrapped device: %d", wrapped_status);

        free(wrapper);

        dlclose(wrapped_module_handle);

        return wrapped_status;
    }

    wrapper->wrapped_device = wrapped_device;

    *device = (struct hw_device_t*) wrapper;

    return 0;
}

static struct hw_module_methods_t gps_module_methods = {
    .open =  gps_module_open,
};

struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .module_api_version = HARDWARE_MODULE_API_VERSION(1, 0),
    .hal_api_version = HARDWARE_HAL_API_VERSION,
    .id = GPS_HARDWARE_MODULE_ID,
    .name = "fp1 MediaTek GPS wrapper HAL module",
    .author = "Daniel Calviño Sánchez",
    .methods = &gps_module_methods,
};
