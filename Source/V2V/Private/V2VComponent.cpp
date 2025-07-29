// Fill out your copyright notice in the Description page of Project Settings.


#include "V2VComponent.h"


// Sets default values for this component's properties
UV2VComponent::UV2VComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UV2VComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UV2VComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UV2VComponent::TryActivatingUpnp(int port, bool& isUpnpSuccessfull)
{
	// Instantiate UPnPManager
	UpnpInstance = new UpnpManager();

	// Initialize UPnP (perform discovery)
	if (!UpnpInstance->Initialize())
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, TEXT("UPnP initialization failed because your router/ISP does not support UPnP."));
		UE_LOG(LogTemp, Error, TEXT("MINIUPNP:: UPnP initialization failed because your router/ISP does not support UPnP."));
		delete UpnpInstance;
		UpnpInstance = nullptr;
		isUpnpSuccessfull = false;
		return;
	}

	// Attempt to add port mapping using UDP
	if (UpnpInstance->AddPortMapping(port, "UDP") && UpnpInstance->AddPortMapping(port, "TCP"))
	{
		UE_LOG(LogTemp, Log, TEXT("MINIUPNP:: Server hosting started with UPnP port forwarding."));
		GEngine->AddOnScreenDebugMessage(-1, 150, FColor::Green, TEXT("UPnP port forwarded"));
		isUpnpSuccessfull = true;
	}
	else
	{
		// Handle failure
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, TEXT("Automatic port forwarding failed (router/ISP limitation). Please forward port 7777 to this PC’s local IP address"));
		UE_LOG(LogTemp, Error, TEXT("MINIUPNP:: Automatic port forwarding failed (router/ISP limitation). Please forward port 7777 to this PC’s local IP address"));
		delete UpnpInstance;
		UpnpInstance = nullptr;
		isUpnpSuccessfull = false;
	}
}

void UV2VComponent::EndUpnpActivation()
{
	if (UpnpInstance)
	{
		int32 Port = 7777; // Replace with your game's port
		if (UpnpInstance->RemovePortMapping(Port, "UDP") && UpnpInstance->RemovePortMapping(Port, "TCP"))
		{
			UE_LOG(LogTemp, Log, TEXT("MINIUPNP:: Port mapping removed successfully."));
			GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Green, TEXT("Port mapping removed successfully."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MINIUPNP:: Failed to remove port mapping."));
			GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, TEXT("Failed to remove port mapping."));
		}

		delete UpnpInstance;
		UpnpInstance = nullptr;
	}
}

void UV2VComponent::HostServer(UGameInstance* GameInstance, const FString& MapName, int32 Port, bool bEnableListenServer, bool bAbsolute)
{
	if (!GameInstance) return;
	UWorld* World = GameInstance->GetWorld();
	if (World)
	{
		GameInstance->EnableListenServer(bEnableListenServer, Port);
		World->ServerTravel(MapName, bAbsolute, false);
	}
}

void UV2VComponent::JoinServer(UGameInstance* GameInstance, FString Ip)
{
	if (!GameInstance) return;
	UWorld* World = GameInstance->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is null. Cannot perform ClientTravel."));
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, TEXT("World is null. Cannot perform ClientTravel."));
		return;
	}
	
	APlayerController* PlayerController = World->GetFirstPlayerController();

	if (PlayerController)
	{
		// Initiate client travel
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Green, Ip);
		PlayerController->ClientTravel(Ip, ETravelType::TRAVEL_Absolute);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController not found."));
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, TEXT("PlayerController not found."));
	}
}

void UV2VComponent::CopyTextToClipboard(FString Text)
{
	FPlatformApplicationMisc::ClipboardCopy(*Text);
}


FString UV2VComponent::GetExternalIP()
{
	if(!ExternalIP.IsEmpty())
	{
		return ExternalIP;
	}
	
	// Create HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UV2VComponent::OnResponseReceived);
    
	// Set the URL to an IP-check service
	Request->SetURL("https://api.ipify.org?format=json");
	Request->SetVerb("GET");
	Request->ProcessRequest();
	return ExternalIP; 
}

void UV2VComponent::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		FString JsonResponse = Response->GetContentAsString();
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonResponse);

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			ExternalIP = JsonObject->GetStringField("ip");
			UE_LOG(LogTemp, Log, TEXT("External IP: %s"), *ExternalIP);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get external IP"));
	}
}



