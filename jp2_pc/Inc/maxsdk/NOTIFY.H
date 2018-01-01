/**********************************************************************
 *<
	FILE: notify.h

	DESCRIPTION: Include file for event notification support

	CREATED BY: Tom Hudson

	HISTORY: Created 8 April 1995

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef _NOTIFY_H_

#define _NOTIFY_H_

// Pre-defined Jaguar system notification codes

#define NOTIFY_UNITS_CHANGE			0x00000001
#define NOTIFY_TIMEUNITS_CHANGE	 	0x00000002
#define NOTIFY_VIEWPORT_CHANGE	 	0x00000003
#define NOTIFY_SPACEMODE_CHANGE	 	0x00000004

// Notification information structure -- Passed to NOTIFYPROC to inform it what
// it's being notified about...
typedef struct {
	int intcode;		// Zero if user code
	} NotifyInfo;

// The notification callback function
typedef void (* NOTIFYPROC)(void *param, NotifyInfo *info);

// Integer versions -- For pre-defined Jaguar codes
int CoreExport RegisterNotification(NOTIFYPROC proc, void *param, int code);
int CoreExport UnRegisterNotification(NOTIFYPROC proc, void *param, int code);
void CoreExport BroadcastNotification(int code);

// Unregister a callback from all codes
int CoreExport UnRegisterNotification(NOTIFYPROC proc, void *param);

#endif // _NOTIFY_H_
