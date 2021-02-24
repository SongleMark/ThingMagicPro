//============================================================================
// Name        : libitkcpp_event.h
// Author      : Jean-Michel Bichoux
// Version     : 1.0.0.0 
// Copyright   : Boingtech 2018
// Description : Header for libitkcpp event
//============================================================================

#pragma once
#ifndef __LIBITKCPP_EVENT_H_
#define __LIBITKCPP_EVENT_H_

#pragma region include 
# include <string>
#include <time.h>
#include <sys/time.h>
#include "event_types.h"
#pragma endregion include 

using namespace std;

class LibITKCPPEvent
{
#pragma region public
public:
	//LibITKCPPEvent();
	LibITKCPPEvent(string Source, string Method, string msg);
	virtual ~LibITKCPPEvent();

	int		EventType;	
	string  Message;	
	string	Source;
	string	Method;
	string	DateTime;
#pragma endregion public
};

#endif /* __LIBITKCPP_EVENT_H_ */
