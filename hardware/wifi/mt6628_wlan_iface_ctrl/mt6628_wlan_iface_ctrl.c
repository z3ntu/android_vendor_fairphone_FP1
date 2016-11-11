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

#define LOG_TAG "mt6628_wlan_iface_ctrl"

#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

#include <linux/socket.h>
#include <linux/wireless.h>

#include <cutils/log.h>
#include <cutils/properties.h>

// From "mediatek/kernel/drivers/combo/drv_wlan/mt6628/wlan/os/linux/include/gl_wext_priv.h"
#define IOCTL_SET_INT        (SIOCIWFIRSTPRIV+0)
#define PRIV_CMD_P2P_MODE    28

#define CMD_REMOVE 0
#define CMD_ADD 1

/**
 * Helper command to add and remove the wlan0 and p2p0 net interfaces.
 *
 * Adding and removing the net interfaces is performed by the kernel; this
 * helper command just sends the needed orders to the kernel to add or remove
 * them.
 *
 * The wlan0 interface is controlled by writing to the "/dev/wmtWifi" device,
 * while the p2p0 interface is controlled by sending a private ioctl to a
 * socket. In both cases, adding or removing the net interfaces in the end is
 * performed by the wlan_mt6628.ko kernel module; this helper command expects
 * that module to be already loaded when executed. Moreover, the "/dev/wmtWifi"
 * device is provided by the "mtk_wmt_wifi.ko" module, and it must be created
 * explicitly by using "mknod /dev/wmtWifi c 153 0"; this helper command also
 * expects the "/dev/wmtWifi" to be already created when executed.
 *
 * When this helper command is executed a system property name can be provided
 * to store in it the result of the execution (either "ok" or "failed"). This
 * makes possible to define a one shot service for this command and be able to
 * check its result once finished.
 *
 * As this helper command is expected to be used through an init service its
 * output is written to the Android log instead of to the standard console.
 */

static int is_cmd_needed(int cmd, const char* interface) {
    char interface_path[256];
    snprintf(interface_path, sizeof(interface_path), "/sys/class/net/%s", interface);

    int interface_exists = (access(interface_path, F_OK) == 0);

    if (interface_exists && cmd == CMD_ADD) {
        ALOGI("Interface %s has been added already", interface);

        return 0;
    }

    if (!interface_exists && cmd == CMD_REMOVE) {
        ALOGI("Interface %s has been removed already", interface);

        return 0;
    }

    return 1;
}

static int ctrl_wlan0(int cmd) {
    if (!is_cmd_needed(cmd, "wlan0")) {
        return 0;
    }

    int fd = open("/dev/wmtWifi", O_WRONLY);
    if (fd < 0) {
        ALOGE("Could not open /dev/wmtWifi to control the wlan0 interface: %s", strerror(errno));

        return -errno;
    }

    char wmtWifi_cmd[2];
    if (cmd == CMD_ADD) {
        strcpy(wmtWifi_cmd, "1");
    } else {
        strcpy(wmtWifi_cmd, "0");
    }

    if (write(fd, &wmtWifi_cmd, sizeof(wmtWifi_cmd)) < 0) {
        ALOGE("Could not write to /dev/wmtWifi to control the wlan0 interface: %s", strerror(errno));

        close(fd);

        return -errno;
    }

    close(fd);

    return 0;
}

static int ctrl_p2p0(int cmd) {
    if (!is_cmd_needed(cmd, "p2p0")) {
        return 0;
    }

    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        ALOGE("Could not create socket to control the p2p0 interface: %s", strerror(errno));

        return -errno;
    }

    struct iwreq ioctl_data;
    memset(&ioctl_data, 0, sizeof(ioctl_data));

    strncpy(ioctl_data.ifr_ifrn.ifrn_name, "wlan0", IFNAMSIZ);

    ioctl_data.u.mode = PRIV_CMD_P2P_MODE;

    // See "priv_set_int" in "mediatek/kernel/drivers/combo/drv_wlan/mt6628/wlan/os/linux/gl_wext_priv.c"
    // and "wlanoidSetP2pMode" in "mediatek/kernel/drivers/combo/drv_wlan/mt6628/wlan/common/wlan_oid.c".
    uint32_t* ioctl_data_extra = (uint32_t*) &ioctl_data.u;
    if (cmd == CMD_ADD) {
        ioctl_data_extra[1] = 1;
        ioctl_data_extra[2] = 0;
    } else {
        ioctl_data_extra[1] = 0;
    }

    if (ioctl(fd, IOCTL_SET_INT, &ioctl_data) < 0) {
        ALOGE("Could not send ioctl to control the p2p0 interface: %s", strerror(errno));

        close(fd);

        return -errno;
    }

    close(fd);

    return 0;
}

int add_interfaces() {
    if (ctrl_wlan0(CMD_ADD) < 0) {
        ALOGE("Could not add the wlan0 interface");

        return 1;
    }

    if (ctrl_p2p0(CMD_ADD) < 0) {
        ALOGE("Could not add the p2p0 interface");

        return 1;
    }

    return 0;
}

int remove_interfaces() {
    int failure = 0;

    if (ctrl_p2p0(CMD_REMOVE) < 0) {
        ALOGE("Could not remove the p2p0 interface");

        failure = 1;
    }

    if (ctrl_wlan0(CMD_REMOVE) < 0) {
        ALOGE("Could not remove the wlan0 interface");

        failure = 1;
    }

    return failure;
}

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        ALOGE("Usage: %s add|remove [property name for result]", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "add") != 0 && strcmp(argv[1], "remove") != 0) {
        ALOGE("Value must be 'add' or 'remove'; value given: '%s'", argv[1]);
        return 1;
    }

    int ret;
    if (!strcmp(argv[1], "add")) {
        ret = add_interfaces();
    } else {
        ret = remove_interfaces();
    }

    if (argc == 3) {
        if (ret) {
            property_set(argv[2], "failed");
        } else {
            property_set(argv[2], "ok");
        }
    }

    return ret;
}
