// Add permissions for setting system properties. See
// "system/core/init/property_service.c".
#define PROPERTY_PERMS_APPEND \
    { "nvram_init", AID_NVRAM, 0 },
