/*
** Copyright (c) 2016 CognitiveVR, Inc. All rights reserved.
*/

#include "network.h"

Network::Network()
{
	Gateway = FAnalytics::Get().GetConfigValueFromIni(GEngineIni, "/Script/CognitiveVR.CognitiveVRSettings", "Gateway", false);
	cog = FAnalyticsCognitiveVR::Get().GetCognitiveVRProvider().Pin();
	Http = &FHttpModule::Get();
	hasErrorResponse = false;
}

void Network::NetworkCall(FString suburl, FString contents)
{
	if (!cog.IsValid())
	{
		CognitiveLog::Warning("FAnalyticsProviderCognitiveVR::SendJson CognitiveVRProvider has not started a session!");
		return;
	}

	if (cog->GetCurrentSceneId().Len() == 0)
	{
		CognitiveLog::Warning("FAnalyticsProviderCognitiveVR::SendJson CognitiveVRProvider has not started a session!");
		return;
	}
	if (cog->GetCurrentSceneVersionNumber().Len() == 0)
	{
		CognitiveLog::Warning("FAnalyticsProviderCognitiveVR::SendJson CognitiveVRProvider has not started a session!");
		return;
	}

	if (Http == NULL)
	{
		CognitiveLog::Warning("Cognitive Provider::SendJson Http module not initialized! likely hasn't started session");
		return;
	}

	//json to scene endpoint
	FString url = "https://"+ Gateway +"/v"+FString::FromInt(0)+"/"+suburl+"/"+cog->GetCurrentSceneId() + "?version=" + cog->GetCurrentSceneVersionNumber();

	FString AuthValue = "APIKEY:DATA " + cog->ApplicationKey;

	TSharedRef<IHttpRequest> HttpRequest = Http->CreateRequest();
	HttpRequest->SetContentAsString(contents);
	HttpRequest->SetURL(url);
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
	HttpRequest->SetHeader("Authorization", AuthValue);
	if (CognitiveLog::DevLogEnabled())
		HttpRequest->OnProcessRequestComplete().BindRaw(this, &Network::OnCallReceivedAsync);
	HttpRequest->ProcessRequest();

	if (CognitiveLog::DevLogEnabled())
		CognitiveLog::DevLog(url + "\n" + contents);
}

void Network::OnCallReceivedAsync(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (Response.IsValid())
	{
		int32 responseCode = Response.Get()->GetResponseCode();
		CognitiveLog::DevLog("Network::OnCallReceivedAsync " + FString::FromInt(responseCode));

		if (responseCode != 200)
		{
			hasErrorResponse = true;
		}
	}
	else
	{
		CognitiveLog::DevLog("Network::OnCallReceivedAsync Response Invalid!");
	}
}

bool Network::HasErrorResponse()
{
	return hasErrorResponse;
}

void Network::NetworkExitPollGetQuestionSet(FString hook, FCognitiveExitPollResponse& response)
{
	TSharedRef<IHttpRequest> HttpRequest = Http->CreateRequest();
	
	FString AuthValue = "APIKEY:DATA " + cog->ApplicationKey;
	
	FString url = "https://" + Gateway + "/v" + FString::FromInt(0) + "/questionSetHooks/" + hook + "/questionSet";

	HttpRequest->SetURL(url);
	HttpRequest->SetVerb("GET");
	HttpRequest->SetHeader("Authorization", AuthValue);
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &Network::OnExitPollResponseReceivedAsync);
	HttpRequest->ProcessRequest();
}

void Network::OnExitPollResponseReceivedAsync(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!Response.IsValid() || !bWasSuccessful)
	{
		CognitiveLog::Error("Network::OnExitPollResponseReceivedAsync - No valid Response. Check internet connection");
		cog->exitpoll->OnResponseReceived("", false);
		return;
	}
	cog->exitpoll->OnResponseReceived(Response->GetContentAsString(), true);
}

void Network::NetworkExitPollPostResponse(FExitPollQuestionSet currentQuestionSet, FExitPollResponse Responses)
{
	if (!cog.IsValid() || !cog->HasStartedSession())
	{
		CognitiveLog::Error("Network::NetworkExitPollPostResponse could not get provider!");
		return;
	}

	TSharedPtr<FJsonObject> ResponseObject = MakeShareable(new FJsonObject);
	ResponseObject->SetStringField("userId", Responses.userId);
	ResponseObject->SetStringField("participantId", Responses.participantId);
	ResponseObject->SetStringField("questionSetId", Responses.questionSetId);
	ResponseObject->SetStringField("sessionId", Responses.sessionId);
	ResponseObject->SetStringField("hook", Responses.hook);

	auto scenedata = cog->GetCurrentSceneData();
	if (scenedata.IsValid())
	{
		ResponseObject->SetStringField("sceneId", scenedata->Id);
		ResponseObject->SetNumberField("versionNumber", scenedata->VersionNumber);
		ResponseObject->SetNumberField("versionId", scenedata->VersionId);
	}

	TArray<TSharedPtr<FJsonValue>> answerValues;
	

	for (int32 i = 0; i < Responses.answers.Num(); i++)
	{
		TSharedPtr<FJsonObject> answerObject = MakeShareable(new FJsonObject);
		answerObject->SetStringField("type", Responses.answers[i].type);
		if (Responses.answers[i].AnswerValueType == EAnswerValueTypeReturn::String)
		{
			answerObject->SetStringField("value", Responses.answers[i].stringValue);
		}
		else if (Responses.answers[i].AnswerValueType == EAnswerValueTypeReturn::Number)
		{
			answerObject->SetNumberField("value", Responses.answers[i].numberValue);
		}
		else if (Responses.answers[i].AnswerValueType == EAnswerValueTypeReturn::Bool)
		{
			if (Responses.answers[i].boolValue == true)
			{
				answerObject->SetNumberField("value", 1);
			}
			else
			{
				answerObject->SetNumberField("value", 0);
			}
		}
		else if (Responses.answers[i].AnswerValueType == EAnswerValueTypeReturn::Null)
		{
			answerObject->SetNumberField("value", -32768);
		}
		TSharedPtr<FJsonValueObject> ao = MakeShareable(new FJsonValueObject(answerObject));
		answerValues.Add(ao);
	}
	ResponseObject->SetArrayField("answers", answerValues);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(ResponseObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	
	FString url = "https://" + Gateway + "/v" + FString::FromInt(0) + "/questionSets/" + currentQuestionSet.name + "/" + FString::FromInt(currentQuestionSet.version) + "/responses";
	FString AuthValue = "APIKEY:DATA " + cog->ApplicationKey;

	HttpRequest->SetURL(url);
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetHeader("Authorization", AuthValue);
	HttpRequest->SetVerb("POST");
	HttpRequest->SetContentAsString(OutputString);
	HttpRequest->ProcessRequest();

	//send this as a transaction too
	TSharedPtr<FJsonObject> properties = MakeShareable(new FJsonObject);
	properties->SetStringField("userId", Responses.userId);
	properties->SetStringField("participantId", Responses.participantId);
	properties->SetStringField("questionSetId", Responses.questionSetId);
	properties->SetStringField("hook", Responses.hook);
	properties->SetNumberField("duration", Responses.duration);

	for (int32 i = 0; i < Responses.answers.Num(); i++)
	{
		if (Responses.answers[i].AnswerValueType == EAnswerValueTypeReturn::Number)
		{
			properties->SetNumberField("Answer" + FString::FromInt(i), Responses.answers[i].numberValue);
		}
		else if (Responses.answers[i].AnswerValueType == EAnswerValueTypeReturn::Bool) //bool as number
		{
			if (Responses.answers[i].boolValue == true)
			{
				properties->SetNumberField("Answer" + FString::FromInt(i), 1);
			}
			else
			{
				properties->SetNumberField("Answer" + FString::FromInt(i), 0);
			}
		}
		else if (Responses.answers[i].AnswerValueType == EAnswerValueTypeReturn::Null)
		{
			//skipped answer
			properties->SetNumberField("Answer" + FString::FromInt(i), -32768);
		}
		else if (Responses.answers[i].AnswerValueType == EAnswerValueTypeReturn::String)
		{
			//voice answer. don't display on dashboard, but not skipped
			properties->SetNumberField("Answer" + FString::FromInt(i), 0);
		}
	}

	//IMPROVEMENT custom event position should be exitpoll panel position. how to get panel position?
	cog->customEventRecorder->Send(FString("c3d.exitpoll"), FVector(0, 0, 0), properties);

	//then flush transactions
	cog->FlushEvents();
}