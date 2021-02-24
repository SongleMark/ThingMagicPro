//============================================================================
// Name        : error_types.h
// Author      : Jean-Michel Bichoux
// Version     : 1.0.0.0 
// Copyright   : Boingtech 2018
// Description : Header for types of event for libitkcpp
//============================================================================

#pragma once
#ifndef __EVENT_TYPES_H_
#define __EVENT_TYPES_H_

#pragma region define
#define ERROR_EVENT_TYPE		0
#define LIBITKCPP_EVENT_TYPE	100
#pragma endregion define

#pragma region struct EventArg
struct EventArg
{
	int	EventType;
	void *	Event;
};
#pragma endregion struct EventArg

#endif /* __EVENT_TYPES_H_ */

