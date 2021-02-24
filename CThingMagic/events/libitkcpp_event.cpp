//============================================================================
// Name        : libitkcpp_event.cpp
// Author      : Jean-Michel Bichoux
// Version     : 1.0 
// Copyright   : Boingtech 2018
// Description : Event structure for libitkcpp module
//============================================================================

#pragma region include
#include <stdio.h>
#include "libitkcpp_event.h"
#pragma endregion include

//#pragma region Class constructor
//LibITKCPPEvent::LibITKCPPEvent()
//{
//	EventType = LIBITKCPP_EVENT_TYPE;
//}
//#pragma endregion Class constructor

#pragma region LIBITKCPPEvent
LibITKCPPEvent::LibITKCPPEvent(string sSource, string sMethod, string sMessage)
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
	Method		= sMethod;
	Message		= sMessage;
	EventType 	= LIBITKCPP_EVENT_TYPE;
	DateTime 	= time_string;
	DateTime 	+= MS;
}
#pragma endregion LIBITKCPPEvent

#pragma region Class destructor
LibITKCPPEvent::~LibITKCPPEvent()
{
}
#pragma endregion Class destructor
