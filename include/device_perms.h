// Add permissions for setting system properties. See
// "system/core/init/property_service.c".
#define PROPERTY_PERMS_APPEND \
    { "gps.", AID_GPS, 0 }, \
    { "nvram_init", AID_NVRAM, 0 }, \
    { "persist.mtk.wcn.combo.chipid", AID_SYSTEM, 0 },
