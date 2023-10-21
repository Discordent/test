// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpSubsystem.h"

#include <string>

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Evaluation/IMovieSceneEvaluationHook.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonUtils/JsonPointer.h"


UHttpSubsystem::UHttpSubsystem()
{
	
}

void UHttpSubsystem::Login(const FString& ServerUrl, const FString& Login, const FString& Password)
{
	FString Content =GetContent(Login
			, Password
			, "Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:47.0) Gecko/20100101 Firefox/47.0");

	FHttpModule* Module = &FHttpModule::Get();
	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> Request = Module->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UHttpSubsystem::OnPostReceivedResponse);
	Request->SetURL(ServerUrl); 
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetContentAsString(Content);
	Request->ProcessRequest();
}

void UHttpSubsystem::Test(const FString& ServerUrl)
{
	FHttpModule* Module = &FHttpModule::Get();
	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> Request = Module->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UHttpSubsystem::OnReceivedResponse);
	Request->SetURL(ServerUrl); 
	Request->SetVerb("GET");
	Request->SetHeader("Authorization", "Bearer "+ TokenSetting.Access_Token);
	Request->ProcessRequest();
}

void UHttpSubsystem::OnReceivedResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	LogedMsg(Response->GetContentAsString());
}

void UHttpSubsystem::OnPostReceivedResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		if( Response->GetResponseCode() == EHttpResponseCodes::Ok)
		{
			UpdateToken(Response->GetContentAsString());
		}		
	}
	LogedMsg(Response->GetContentAsString());
	ReceivedLoginResponse.Broadcast(bWasSuccessful);
}

FString UHttpSubsystem::GetContent(const FString& Login, const FString& Password, const FString& Fingerprint) const
{	
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("login", Login);
	JsonObject->SetStringField("password", Password);
	JsonObject->SetStringField("fingerprint", Fingerprint);
	
	FString OutputString = JsonObjectToString(JsonObject);
	return  OutputString;
}

FString UHttpSubsystem::JsonObjectToString(const TSharedPtr<FJsonObject> JsonObject) const
{
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	
	CleanJsonString(OutputString);
	return OutputString;
}

void UHttpSubsystem::CleanJsonString(FString& JString) const
{
	JString = JString.Replace(TEXT("\n"), TEXT(""));
	JString = JString.Replace(TEXT("\t"), TEXT(""));
	JString = JString.Replace(TEXT("\r"), TEXT(""));
	JString = JString.Replace(TEXT(" "), TEXT(""));
}

void UHttpSubsystem::UpdateToken(const FString& JString)
{
	FJsonObjectConverter::JsonObjectStringToUStruct(JString, &TokenSetting);
	UE_LOG(LogTemp, Display, TEXT("New Token is : %s"), *TokenSetting.Access_Token);
}

bool UHttpSubsystem::HasToken() const
{
	return !TokenSetting.Access_Token.IsEmpty();
}


void UHttpSubsystem::LogedMsg(const FString& Msg)
{
	TArray< FStringFormatArg > Args;
	Args.Add( FStringFormatArg( FDateTime::Now().ToString() ) );
	Args.Add( FStringFormatArg( Msg ) );

	const FString FormatString = FString::Format( TEXT( "{0} :{1}\n" ), Args );
	UE_LOG(LogTemp, Display, TEXT("%s"), *Msg);
	UpdateLog.Broadcast(FormatString);
}
