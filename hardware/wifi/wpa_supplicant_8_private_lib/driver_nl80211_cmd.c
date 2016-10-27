/*
 * driver_nl80211_cmd.c is based on
 * "hardware/qcom/wlan/qcwcn/wpa_supplicant_8_lib/driver_cmd_nl80211.c".
 *
 * Copyright (C) 2016 Daniel Calviño Sánchez <danxuliu@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/if.h>
#include <linux/wireless.h>

#include <hardware_legacy/driver_nl80211.h>

#include "drivers/android_drv.h"

#include "utils/common.h"
#include "utils/list.h"
#include "utils/os.h"
#include "utils/wpa_debug.h"

// These files depend on types defined in "utils/common.h".
#include "drivers/driver.h"
#include "drivers/linux_ioctl.h"

int wpa_driver_nl80211_driver_cmd(void* priv, char* cmd, char* buf, size_t buf_len) {
    struct i802_bss* bss = priv;
    struct wpa_driver_nl80211_data* drv = bss->drv;
    int ret = 0;

    if (os_strcmp(cmd, "START") == 0) {
        linux_set_iface_flags(drv->global->ioctl_sock, bss->ifname, 1);

        wpa_msg(drv->ctx, MSG_INFO, WPA_EVENT_DRIVER_STATE "STARTED");
    } else if (os_strcmp(cmd, "STOP") == 0) {
        linux_set_iface_flags(drv->global->ioctl_sock, bss->ifname, 0);

        wpa_msg(drv->ctx, MSG_INFO, WPA_EVENT_DRIVER_STATE "STOPPED");
    } else if (os_strcasecmp(cmd, "MACADDR") == 0) {
        u8 macaddr[ETH_ALEN] = {};

        ret = linux_get_ifhwaddr(drv->global->ioctl_sock, bss->ifname, macaddr);
        if (!ret) {
            ret = os_snprintf(buf, buf_len, "Macaddr = " MACSTR "\n", MAC2STR(macaddr));
        }
    } else if (!os_strncmp(cmd, "COUNTRY", os_strlen("COUNTRY"))) {
        struct iwreq ifr;
        memset(&ifr, 0, sizeof(ifr));
        os_strncpy(ifr.ifr_name, bss->ifname, IFNAMSIZ);

        // See "wext_support_ioctl" and "wext_set_country" in
        // "mediatek/kernel/drivers/combo/drv_wlan/mt6628/wlan/os/linux/gl_wext.c".
        ifr.u.data.pointer = cmd;
        ifr.u.data.length = strlen(cmd) + 1;

        if ((ret = ioctl(drv->global->ioctl_sock, SIOCSIWPRIV, &ifr)) < 0) {
            wpa_printf(MSG_ERROR, "Failed to issue 'COUNTRY' private command");
        } else {
            wpa_supplicant_event(drv->ctx, EVENT_CHANNEL_LIST_CHANGED, NULL);
        }
    } else {
        wpa_printf(MSG_WARNING, "Unsupported private command: %s", cmd);
    }

    return ret;
}

int wpa_driver_set_p2p_noa(void *priv, u8 count, int start, int duration) {
    wpa_printf(MSG_WARNING, "wpa_driver_set_p2p_noa is not implemented");

    return -1;
}

int wpa_driver_get_p2p_noa(void *priv, u8 *buf, size_t len) {
    wpa_printf(MSG_WARNING, "wpa_driver_get_p2p_noa is not implemented");

    return -1;
}

int wpa_driver_set_p2p_ps(void *priv, int legacy_ps, int opp_ps, int ctwindow) {
    wpa_printf(MSG_WARNING, "wpa_driver_set_p2p_ps is not implemented");

    return -1;
}

int wpa_driver_set_ap_wps_p2p_ie(void *priv, const struct wpabuf *beacon,
                                 const struct wpabuf *proberesp,
                                 const struct wpabuf *assocresp) {
    wpa_printf(MSG_WARNING, "wpa_driver_set_ap_wps_p2p_ie is not implemented");

    return -1;
}
