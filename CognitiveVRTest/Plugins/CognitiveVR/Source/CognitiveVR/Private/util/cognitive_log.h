/*
** Copyright (c) 2016 CognitiveVR, Inc. All rights reserved.
*/
#pragma once

#include "CognitiveVR.h"
#include <iostream>
#include <ctime>

//#include "CognitiveVR.h"
//#include "CognitiveVRPrivatePCH.h"
//#include "Private/util/config.h"

//#include "CognitiveVR.h"

//namespace cognitivevrapi
//{
    class CognitiveLog
    {
	private:
		static bool ShowDebugLogs; //basic info/warning/errors
		static bool ShowDevLogs; //development specific logs

        public:
			static void Init();
			static void DevLog(FString s);
			static void Info(FString s);
            static void Warning(FString s);
            static void Error(FString s);

			static bool DevLogEnabled();
    };
//}