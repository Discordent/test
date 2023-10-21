// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utils/Types.h"
#include "Interfaces/IHttpRequest.h"
#include "Subsystems/Subsystem.h"
#include "HttpSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReceivedLoginResponse, bool, Status);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateLog, FString, LogMsg);

UCLASS()
class TESTPROJ_API UHttpSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UHttpSubsystem();
	
	UFUNCTION(BlueprintCallable)
	void Login(const FString& ServerUrl, const FString& Login, const FString& Password);

	UFUNCTION(BlueprintCallable)
	void Test(const FString& ServerUrl);
	
	void OnPostReceivedResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void OnReceivedResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	UPROPERTY(BlueprintAssignable)
	FReceivedLoginResponse ReceivedLoginResponse;

	UPROPERTY(BlueprintAssignable)
	FUpdateLog UpdateLog;

	UFUNCTION(BlueprintCallable)
	bool HasToken() const;
	
protected:
	FString GetContent(const FString& Login, const FString& Password, const FString& Fingerprint) const;

	FString JsonObjectToString(const TSharedPtr<FJsonObject> JsonObject) const;
	void CleanJsonString(FString& JString) const;
	void UpdateToken(const FString& JString);

	void LogedMsg(const FString& Msg);
	
	
	FTokenState TokenSetting;	 
};
