// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UpnpManager.h"
#include "Components/ActorComponent.h"
#include "Engine/GameInstance.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include "HttpModule.h"
#include "V2VComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class V2V_API UV2VComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UV2VComponent();

	UFUNCTION(BlueprintCallable, Category = "UPNP|Core")
	void TryActivatingUpnp(int port, bool& isUpnpSuccessfull);

	UFUNCTION(BlueprintCallable, Category = "UPNP|Core")
	void EndUpnpActivation();

	UFUNCTION(BlueprintCallable, Category="UPNP|Server|Connection")
	static void HostServer(UGameInstance* GameInstance, const FString& MapName, int32 Port, bool bEnableListenServer, bool bAbsolute);

	UFUNCTION(BlueprintCallable, Category = "UPNP|Server|Connection")
	static void JoinServer(UGameInstance* GameInstance, FString Ip);

	UFUNCTION(BlueprintCallable, Category = "UPNP|Util")
	static void CopyTextToClipboard(FString Text);

	UFUNCTION(BlueprintCallable, Category = "UPNP|Server|ServerInfo")
	FString GetExternalIP();	

	UPROPERTY(BlueprintReadOnly, Category = "UPNP|Server|ServerInfo")
	FString ExternalIP;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UpnpManager* UpnpInstance;
	
	
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);		
};
