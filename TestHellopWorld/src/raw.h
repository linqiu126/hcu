/*
 * raw.h
 *
 *  Created on: 2016年11月17日
 *      Author: hitpony
 */

#ifndef RAW_H_
#define RAW_H_

/*
 * socketcan/can/raw.h
 *
 * Definitions for raw CAN sockets
 *
 * $Id: raw.h 1038 2009-08-21 10:00:21Z hartkopp $
 *
 * Authors: Oliver Hartkopp <oliver.hartkopp@volkswagen.de>
 *          Urs Thuermann   <urs.thuermann@volkswagen.de>
 * Copyright (c) 2002-2007 Volkswagen Group Electronic Research
 * All rights reserved.
 *
 * Send feedback to <socketcan-users@lists.berlios.de>
 *
 */

#ifndef CAN_RAW_H
#define CAN_RAW_H

#include "TestHellopWorld.h"

#define SOL_CAN_RAW (SOL_CAN_BASE + CAN_RAW)

/* for socket options affecting the socket (not the global system) */

enum {
	CAN_RAW_FILTER = 1,	/* set 0 .. n can_filter(s)          */
	CAN_RAW_ERR_FILTER,	/* set filter for error frames       */
	CAN_RAW_LOOPBACK,	/* local loopback (default:on)       */
	CAN_RAW_RECV_OWN_MSGS	/* receive my own msgs (default:off) */
};

#endif


#endif /* RAW_H_ */
