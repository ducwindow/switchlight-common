/****************************************************************
 *
 *        Copyright 2013, Big Switch Networks, Inc.
 *
 * Licensed under the Eclipse Public License, Version 1.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *        http://www.eclipse.org/legal/epl-v10.html
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the
 * License.
 *
 ****************************************************************/
#ifndef __LACPA_UTILS_H__
#define __LACPA_UTILS_H__

#include "lacpa_int.h"
#include <PPE/ppe.h>

/*
 * lacpa_copy_mac
 */
static inline void
lacpa_copy_mac (lacpa_mac_t from, lacpa_mac_t to)
{
    if (!from || !to) return;
    LACPA_MEMCPY(to, from, MAC_ADDRESS_BYTES);
}

/*
 * lacpa_copy_info
 */
static inline void
lacpa_copy_info (lacpa_info_t *from, lacpa_info_t *to)
{
    if (!from || !to) return;

    to->sys_priority          = from->sys_priority;
    lacpa_copy_mac(from->sys_mac, to->sys_mac);
    to->port_priority         = from->port_priority;
    to->port_num              = from->port_num;
    to->key                   = from->key;
    to->state                 = from->state;
    to->port_no               = from->port_no;
}

/*
 * lacpa_same_partner
 */
static inline bool
same_partner(lacpa_info_t *a, lacpa_info_t *b)
{
    if (!a || !b) return FALSE;

    return ((a->sys_priority           == b->sys_priority)
            && (!memcmp(a->sys_mac, b->sys_mac, MAC_ADDRESS_BYTES))
            && (a->port_priority       == b->port_priority)
            && (a->port_num            == b->port_num)
            && (a->key                 == b->key));
}

/*
 * lacpa_parse_pdu
 */
static inline bool
lacpa_parse_pdu (ppe_packet_t *ppep, lacpa_pdu_t *pdu)
{
	uint32_t actor_tmp = 0, partner_tmp = 0;

	if (!ppep || !pdu) return FALSE;

    /*
     * Get Actor and Partner Info and Info length
     */
	ppe_field_get(ppep, PPE_FIELD_LACP_ACTOR_INFO, &actor_tmp);
	ppe_field_get(ppep, PPE_FIELD_LACP_PARTNER_INFO, &partner_tmp);

    if (actor_tmp != DEFAULT_ACTOR_INFO ||
        partner_tmp != DEFAULT_PARTNER_INFO) {
        AIM_LOG_TRACE("Bad Parameters - actor_info: 0x%02x, partner_info: "
                      "0x%02x", actor_tmp, partner_tmp);
        return FALSE;
    }

    actor_tmp = 0, partner_tmp = 0;
	ppe_field_get(ppep, PPE_FIELD_LACP_ACTOR_INFO_LEN, &actor_tmp);
	ppe_field_get(ppep, PPE_FIELD_LACP_PARTNER_INFO_LEN, &partner_tmp);

    if (actor_tmp != DEFAULT_ACTOR_PARTNER_INFO_LEN ||
		partner_tmp != DEFAULT_ACTOR_PARTNER_INFO_LEN) {
        AIM_LOG_TRACE("Bad Parameters - actor_info_len: 0x%02x, "
                      "partner_info_len: 0x%02x", actor_tmp, partner_tmp);
        return FALSE;
    }

    /*
     * Get Actor and Partner System Priority and Mac
     */
    actor_tmp = 0, partner_tmp = 0;
    ppe_field_get(ppep, PPE_FIELD_LACP_ACTOR_SYS_PRI, &actor_tmp);
    ppe_field_get(ppep, PPE_FIELD_LACP_PARTNER_SYS_PRI, &partner_tmp);
    ppe_wide_field_get(ppep, PPE_FIELD_LACP_ACTOR_SYS, pdu->actor.sys_mac);
    ppe_wide_field_get(ppep, PPE_FIELD_LACP_PARTNER_SYS, pdu->partner.sys_mac);

    pdu->actor.sys_priority = (uint16_t) actor_tmp;
    pdu->partner.sys_priority = (uint16_t) partner_tmp;

    AIM_LOG_TRACE("Received actor_sys_prio: %d, actor_sys_mac: %{mac}",
                  pdu->actor.sys_priority, pdu->actor.sys_mac);
    AIM_LOG_TRACE("Received partner_sys_prio: %d, partner_sys_mac: %{mac}",
                  pdu->partner.sys_priority, pdu->partner.sys_mac);

    /*
     * Get Actor and Partner Key's
     */
    actor_tmp = 0, partner_tmp = 0;
    ppe_field_get(ppep, PPE_FIELD_LACP_ACTOR_KEY, &actor_tmp);
    ppe_field_get(ppep, PPE_FIELD_LACP_PARTNER_KEY, &partner_tmp);

    pdu->actor.key = (uint16_t) actor_tmp;
    pdu->partner.key = (uint16_t) partner_tmp;

    AIM_LOG_TRACE("Received actor_key: %d, partner_key: %d",
                  pdu->actor.key, pdu->partner.key);

    /*
     * Get Actor and Partner Port Priority and Num
     */
    actor_tmp = 0, partner_tmp = 0;
    ppe_field_get(ppep, PPE_FIELD_LACP_ACTOR_PORT_PRI, &actor_tmp);
    ppe_field_get(ppep, PPE_FIELD_LACP_PARTNER_PORT_PRI, &partner_tmp);

    pdu->actor.port_priority = (uint16_t) actor_tmp;
    pdu->partner.port_priority = (uint16_t) partner_tmp;

    actor_tmp = 0, partner_tmp = 0;
    ppe_field_get(ppep, PPE_FIELD_LACP_ACTOR_PORT, &actor_tmp);
    ppe_field_get(ppep, PPE_FIELD_LACP_PARTNER_PORT, &partner_tmp);

    pdu->actor.port_num = (uint16_t) actor_tmp;
    pdu->partner.port_num = (uint16_t) partner_tmp;

    AIM_LOG_TRACE("Received actor_port_prio: %d, actor_port_num: %d",
                  pdu->actor.port_priority, pdu->actor.port_num);
    AIM_LOG_TRACE("Received partner_port_prio: %d, partner_port_num: %d",
                  pdu->partner.port_priority, pdu->partner.port_num);

    /*
     * Get Actor and Partner State
     */
    actor_tmp = 0, partner_tmp = 0;
    ppe_field_get(ppep, PPE_FIELD_LACP_ACTOR_STATE, &actor_tmp);
    ppe_field_get(ppep, PPE_FIELD_LACP_PARTNER_STATE, &partner_tmp);

    pdu->actor.state =  (lacpa_state_t) actor_tmp;
    pdu->partner.state =  (lacpa_state_t) partner_tmp;

    AIM_LOG_TRACE("Received actor_state: 0x%02x, partner_state: 0x%02x",
                  pdu->actor.state, pdu->partner.state);

    return TRUE;
}

static inline bool
lacpa_build_pdu (ppe_packet_t *ppep, lacpa_port_t *port)
{
    if (!ppep || !port) return FALSE;

    ppe_field_set(ppep, PPE_FIELD_LACP_VERSION, DEFAULT_LACP_VERSION);

    /*
     * Set Actor Parameters
     */
    ppe_field_set(ppep, PPE_FIELD_LACP_ACTOR_INFO, DEFAULT_ACTOR_INFO);
    ppe_field_set(ppep, PPE_FIELD_LACP_ACTOR_INFO_LEN,
                  DEFAULT_ACTOR_PARTNER_INFO_LEN);
    ppe_field_set(ppep, PPE_FIELD_LACP_ACTOR_SYS_PRI,
                  port->actor.sys_priority);
    ppe_wide_field_set(ppep, PPE_FIELD_LACP_ACTOR_SYS, port->actor.sys_mac);
    ppe_field_set(ppep, PPE_FIELD_LACP_ACTOR_KEY, port->actor.key);
    ppe_field_set(ppep, PPE_FIELD_LACP_ACTOR_PORT_PRI,
                  port->actor.port_priority);
    ppe_field_set(ppep, PPE_FIELD_LACP_ACTOR_PORT, port->actor.port_num);
    ppe_field_set(ppep, PPE_FIELD_LACP_ACTOR_STATE, port->actor.state);
    ppe_field_set(ppep, PPE_FIELD_LACP_RSV0, DEFAULT_ZERO);

    /*
     * Set Partner Parameters
     */
    ppe_field_set(ppep, PPE_FIELD_LACP_PARTNER_INFO, DEFAULT_PARTNER_INFO);
    ppe_field_set(ppep, PPE_FIELD_LACP_PARTNER_INFO_LEN,
                  DEFAULT_ACTOR_PARTNER_INFO_LEN);
    ppe_field_set(ppep, PPE_FIELD_LACP_PARTNER_SYS_PRI,
                  port->partner.sys_priority);
    ppe_wide_field_set(ppep, PPE_FIELD_LACP_PARTNER_SYS, port->partner.sys_mac);
    ppe_field_set(ppep, PPE_FIELD_LACP_PARTNER_KEY, port->partner.key);
    ppe_field_set(ppep, PPE_FIELD_LACP_PARTNER_PORT_PRI,
                  port->partner.port_priority);
    ppe_field_set(ppep, PPE_FIELD_LACP_PARTNER_PORT, port->partner.port_num);
    ppe_field_set(ppep, PPE_FIELD_LACP_PARTNER_STATE, port->partner.state);
    ppe_field_set(ppep, PPE_FIELD_LACP_RSV1, DEFAULT_ZERO);

    /*
     * Set other non-essential fields
     */
    ppe_field_set(ppep, PPE_FIELD_LACP_COLLECTOR_INFO, DEFAULT_COLLECTOR_INFO);
    ppe_field_set(ppep, PPE_FIELD_LACP_COLLECTOR_INFO_LEN,
                  DEFAULT_COLLECTOR_INFO_LEN);
    ppe_field_set(ppep, PPE_FIELD_LACP_COLLECTOR_MAX_DELAY,
                  DEFAULT_COLLECTOR_MAX_DELAY);
    //ppe_field_set(ppep, PPE_FIELD_LACP_RSV2, DEFAULT_ZERO);
    ppe_field_set(ppep, PPE_FIELD_LACP_TERMINATOR_INFO, DEFAULT_ZERO);
    ppe_field_set(ppep, PPE_FIELD_LACP_TERMINATOR_LENGTH, DEFAULT_ZERO);
    //ppe_field_set(ppep, PPE_FIELD_LACP_RSV4, DEFAULT_ZERO);

    return TRUE;
}

#endif /* __LACPA_UTILS_H__ */
