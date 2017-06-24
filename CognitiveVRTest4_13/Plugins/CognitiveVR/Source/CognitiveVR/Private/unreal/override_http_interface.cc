/*
** Copyright (c) 2016 CognitiveVR, Inc. All rights reserved.
*/
#include "override_http_interface.h"

using namespace cognitivevrapi;

OverrideHttpInterface::OverrideHttpInterface()
{

}

void OverrideHttpInterface::OnResponseReceivedAsync(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, NetworkCallback callback)
{
	if (!Response.IsValid()) { return; }
    FString UE4Str = Response->GetContentAsString(); //try repo for very rare crash bug. sharedpointer exception. this only fires if callback != null, so only on init?
	std::string content(TCHAR_TO_UTF8(*UE4Str));
	CognitiveLog::Info("OverrideHttpInterface::OnResponseReceivedAsync - Response: " + content);
        
	CognitiveVRResponse response = Network::ParseResponse(content);
    callback(response);
}

std::string OverrideHttpInterface::Post(std::string url, std::string path, std::string headers[], int32 header_count, std::string stdcontent, long timeout, NetworkCallback callback)
{
    //Construct URL.
    std::string stdfull_url = url + path;
    FString full_url(stdfull_url.c_str());
	CognitiveLog::Info("Override_Http_Interface::Post " + stdfull_url);// +"\n" + stdcontent);

    FString content(stdcontent.c_str());

    TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
    FHttpModule::Get().SetHttpTimeout((float)(timeout + 5));

    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));

    //Add headers.
    for (int32 i=0; i<header_count; i++) {
        std::string header = headers[i];

        int32 del_pos = header.find(":");
        std::string key = header.substr(0, del_pos);
        key = Util::Trim(key);
        std::string value = header.substr(del_pos + 1, header.size());
        value = Util::Trim(value);

        FString uekey(key.c_str());
        FString uevalue(value.c_str());
        HttpRequest->SetHeader(uekey, uevalue);
    }

    HttpRequest->SetURL(full_url);
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetContentAsString(content);

	if (callback != NULL) {
		HttpRequest->OnProcessRequestComplete().BindRaw(this, &OverrideHttpInterface::OnResponseReceivedAsync, callback);
	}

	bool process_result = HttpRequest->ProcessRequest();
		
    if (!process_result) {
		cognitivevrapi::CognitiveLog::Error("OverrideHttpInterface::Post - Process Request Failed!");
    }

	this->http_response = "";
	return this->http_response;
}