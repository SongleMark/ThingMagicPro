//============================================================================
// Name        : ErrorEvent.cpp
// Author      : Jean-Michel Bichoux
// Version     : 1.0 (under dev)
// Copyright   : BoingTech 2010-2018
// Description : Event structure for Error module
//============================================================================

#pragma region include
#include <stdio.h>
#include "error_event.h"
#pragma endregion include

//#pragma region Class constructor
//ErrorEvent::ErrorEvent()
//{
//	EventType = ERROR_EVENT_TYPE;
//}
//#pragma endregion Class constructor

#pragma region Destructor
ErrorEvent::~ErrorEvent()
{
}
#pragma endregion Destructor

#pragma region ErrorEvent
//---------------------------------------------------------------------------
// Name        : ErrorEvent()
// Param       : string sSource, string sMessage, int iID, int iCode
// Return      : ErrorEvent
// Description : Event to manage error
//---------------------------------------------------------------------------
ErrorEvent::ErrorEvent(string sSource, string sMessage, int iCode)
{	
	struct	timeval tv;
	struct	tm* ptm;
	char 	time_string[50];
	char 	MS[5] = "";
				
	gettimeofday(&tv, NULL);
	ptm = localtime(&tv.tv_sec);
	sprintf(MS, "%03d", (int)(tv.tv_usec / 1000));
	strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S.", ptm);
	
	Source		= sSource;
	Message		= sMessage;
	Code		= iCode;
	EventType	= ERROR_EVENT_TYPE;
	DateTime	= time_string;
	DateTime	+= MS;
}
#pragma endregion ErrorEvent
