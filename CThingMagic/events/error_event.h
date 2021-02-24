//============================================================================
// Name        : error_event.h
// Author      : Jean-Michel Bichoux
// Version     : 1.0.0.0 
// Copyright   : Boingtech 2018
// Description : Header for error event
//============================================================================

#pragma once
#ifndef __ERROR_EVENT_H_
#define __ERROR_EVENT_H_

#pragma region include
#include <string>
#include <time.h>
#include <sys/time.h>
#include "event_types.h"
#pragma endregion include

using namespace std;

class ErrorEvent
{
#pragma region public
public:
	//ErrorEvent();
	ErrorEvent(string sSource, string sMessage, int iCode);
	virtual ~ErrorEvent();
	
	int EventType;
	int Code;
	string Message;
	string Source;
	string DateTime;
#pragma endregion public
};

#endif /* __ERROR_EVENT_H_ */
