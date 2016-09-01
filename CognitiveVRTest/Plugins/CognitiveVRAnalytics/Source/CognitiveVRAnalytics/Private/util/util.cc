/*
** Copyright (c) 2016 CognitiveVR, Inc. All rights reserved.
*/
#include "util/util.h"

namespace cognitivevrapi
{
    long Util::GetTimestamp()
    {
		#pragma warning(push)
		#pragma warning(disable:4244) //Disable warning regarding loss of accuracy, no concern.

		return time(0);
		//http://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c

		#pragma warning(pop)
    }

    std::string Util::GetTimestampStr(long t)
    {
        if (t == 0) {
            t = Util::GetTimestamp();
        }

        return Util::ToString(t);
    }

    template < typename T > std::string Util::ToString( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }

	TSharedPtr<FJsonObject> Util::DeviceScraper(TSharedPtr<FJsonObject> properties)
	{
		properties->SetStringField("cpu brand", FWindowsPlatformMisc::GetCPUBrand());
		properties->SetStringField("cpu vendor", FWindowsPlatformMisc::GetCPUVendor());
		properties->SetNumberField("cores", FWindowsPlatformMisc::NumberOfCores());
		properties->SetStringField("gpu", FWindowsPlatformMisc::GetPrimaryGPUBrand());

		FString osVersionOut;
		FString osSubVersionOut;
		FWindowsPlatformMisc::GetOSVersions(osVersionOut, osSubVersionOut);

		properties->SetStringField("os", osVersionOut+osSubVersionOut);

		return properties;
	}


	void Util::AppendToJsonArray(TSharedPtr<FJsonValueArray> &json, FString &fstring)
	{
		TArray<TSharedPtr<FJsonValue>> ValueArray = json.Get()->AsArray();
		TSharedPtr<FJsonValue> tempVal = MakeShareable(new FJsonValueString(fstring));
		ValueArray.Emplace(tempVal);

		json = MakeShareable(new FJsonValueArray(ValueArray));
	}


	void Util::AppendToJsonArray(TSharedPtr<FJsonValueArray> &json, std::string &stdstring)
	{
		FString fstring = stdstring.c_str();
		TArray<TSharedPtr<FJsonValue>> ValueArray = json.Get()->AsArray();
		TSharedPtr<FJsonValue> tempVal = MakeShareable(new FJsonValueString(fstring));
		ValueArray.Emplace(tempVal);

		json = MakeShareable(new FJsonValueArray(ValueArray));
	}

	void Util::AppendToJsonArray(TSharedPtr<FJsonValueArray> &json, bool &mybool)
	{
		TArray<TSharedPtr<FJsonValue>> ValueArray = json.Get()->AsArray();
		TSharedPtr<FJsonValue> tempVal = MakeShareable(new FJsonValueNumber(mybool));
		ValueArray.Emplace(tempVal);

		json = MakeShareable(new FJsonValueArray(ValueArray));
	}

	void Util::AppendToJsonArray(TSharedPtr<FJsonValueArray> &json, const char &mychar)
	{
		//cognitivevrapi::Log::Error("Util::AppendToJsonArray - char not supported!");
		//FString fstring = mychar.c_str();
		//TSharedPtr<FJsonValue> tempVal = MakeShareable(new FJsonValueString(fstring));
		//TArray<TSharedPtr<FJsonValue>> ValueArray = json.Get()->AsArray();
		//ValueArray.Emplace(tempVal);
	}

	void Util::AppendToJsonArray(TSharedPtr<FJsonValueArray> &json, long &longValue)
	{
		TArray<TSharedPtr<FJsonValue>> ValueArray = json.Get()->AsArray();
		TSharedPtr<FJsonValue> tempVal = MakeShareable(new FJsonValueNumber(longValue));
		ValueArray.Emplace(tempVal);

		json = MakeShareable(new FJsonValueArray(ValueArray));
	}
	void Util::AppendToJsonArray(TSharedPtr<FJsonValueArray> &json, int &intValue)
	{
		TArray<TSharedPtr<FJsonValue>> ValueArray = json.Get()->AsArray();
		TSharedPtr<FJsonValue> tempVal = MakeShareable(new FJsonValueNumber(intValue));
		ValueArray.Emplace(tempVal);

		json = MakeShareable(new FJsonValueArray(ValueArray));
	}
	void Util::AppendToJsonArray(TSharedPtr<FJsonValueArray>& json, double &doubleValue)
	{
		TArray<TSharedPtr<FJsonValue>> ValueArray = json.Get()->AsArray();
		TSharedPtr<FJsonValue> tempVal = MakeShareable(new FJsonValueNumber(doubleValue));
		ValueArray.Emplace(tempVal);

		json = MakeShareable(new FJsonValueArray(ValueArray));
	}
	void Util::AppendToJsonArray(TSharedPtr<FJsonValueArray>& json, TSharedPtr<FJsonObject> & object)
	{
		TArray<TSharedPtr<FJsonValue>> ValueArray = json.Get()->AsArray();
		TSharedPtr<FJsonValue> tempVal = MakeShareable(new FJsonValueObject(object));
		ValueArray.Emplace(tempVal);

		json = MakeShareable(new FJsonValueArray(ValueArray));
	}
}