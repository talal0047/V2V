// Fill out your copyright notice in the Description page of Project Settings.


#include "UpnpManager.h"
#include "Engine/Engine.h"
#include "upnpcommands.h"
#include "upnperrors.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"


UpnpManager::UpnpManager(): bIsInitialized(false)
{
	// Initialize 'urls' and 'data' structures to zero
	memset(&urls, 0, sizeof(urls));
	memset(&data, 0, sizeof(data));

	// Initialize 'lanAddress' and 'wanAddress' to zero
	memset(lanAddress, 0, sizeof(lanAddress));
	memset(wanAddress, 0, sizeof(wanAddress));
}

UpnpManager::~UpnpManager()
{
    const FString BinDir = IPluginManager::Get()
     .FindPlugin(TEXT("V2V")) 
     ->GetBaseDir() /
     TEXT("ThirdParty/Win64");
    FPlatformProcess::PushDllDirectory(*BinDir);

    if (!FPlatformProcess::GetDllHandle(TEXT("miniupnpc.dll")))
    {
        const uint32 Err = FPlatformMisc::GetLastError();
        TCHAR Msg[512]{}; FPlatformMisc::GetSystemErrorMessage(Msg, UE_ARRAY_COUNT(Msg), Err);
        UE_LOG(LogTemp, Error, TEXT("%s"), Msg);
    }
    
	if (bIsInitialized)
	{
		FreeUPNPUrls(&urls);
		FString Message = FString(TEXT("MINIUPNP:: UPnP URLs freed successfully."));
		UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 150.0f, FColor::Green, Message);
		}
	}
}

bool UpnpManager::Initialize()
{
    int discoverError = 0;

    // Discover UPnP devices with a 2000ms timeout
    struct UPNPDev* devList = upnpDiscover(2000, nullptr, nullptr, 0, 0, 2, &discoverError);
    if (!devList)
    {
        FString ErrorMsg = FString::Printf(TEXT("MINIUPNP:: UPnP discovery failed. Error code: %d"), discoverError);
        UE_LOG(LogTemp, Warning, TEXT("%s"), *ErrorMsg);
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 150.0f, FColor::Red, ErrorMsg);
        }
        return false;
    }

    // Validate IGD (Internet Gateway Device)
    int IGDType = UPNP_GetValidIGD(devList, &urls, &data, lanAddress, sizeof(lanAddress), wanAddress, sizeof(wanAddress));
    freeUPNPDevlist(devList); // Free the device list after validation

    if (IGDType != 1)
    {
        FString WarningMsg = FString::Printf(TEXT("MINIUPNP:: No valid IGD found. Result code: %d"), IGDType);
        UE_LOG(LogTemp, Warning, TEXT("%s"), *WarningMsg);
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 150.0f, FColor::Red, WarningMsg);
        }
        return false;
    }

    FString SuccessMsg = FString::Printf(TEXT("MINIUPNP:: UPnP discovery successful. LAN: %s, WAN: %s"), ANSI_TO_TCHAR(lanAddress), ANSI_TO_TCHAR(wanAddress));
    UE_LOG(LogTemp, Log, TEXT("%s"), *SuccessMsg);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 150.0f, FColor::Green, SuccessMsg);
    }
    bIsInitialized = true;
    return true;
}

bool UpnpManager::AddPortMapping(int32 Port, const FString& Protocol)
{
    if (!bIsInitialized)
    {
        FString WarningMsg = FString(TEXT("MINIUPNP:: UPnPManager not initialized. Call Initialize() first."));
        UE_LOG(LogTemp, Warning, TEXT("%s"), *WarningMsg);
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 150.0f, FColor::Red, WarningMsg);
        }
        return false;
    }

    if (strlen(lanAddress) == 0)
    {
        FString WarningMsg = FString(TEXT("MINIUPNP:: LAN Address not initialized."));
        UE_LOG(LogTemp, Warning, TEXT("%s"), *WarningMsg);
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 150.0f, FColor::Red, WarningMsg);
        }
        return false;
    }

    char ExtPort[6] = {};
    char IntPort[6] = {};
    snprintf(ExtPort, sizeof(ExtPort), "%d", Port);
    snprintf(IntPort, sizeof(IntPort), "%d", Port);

    const char* Desc = "Unreal Engine 5 Game";
    const char* Proto = TCHAR_TO_ANSI(*Protocol);

    // 2. 1st attempt – RFC-compliant
    const char* RemoteHost = "";      // empty string = wildcard (most routers OK)
    const char* Lease = "0";     // "0" = permanent
    int AddRes = UPNP_AddPortMapping(
        urls.controlURL,
        data.first.servicetype,
        ExtPort,
        IntPort,
        lanAddress,
        Desc,
        Proto,
        RemoteHost,
        Lease);

    // 3. Retry for picky routers that hate "" / require NULL
    if (AddRes == 716 /* WildCardNotPermittedInExtPort */
        || AddRes == 725 /* OnlyPermanentLeasesSupported */)
    {
        RemoteHost = nullptr;         // omit <NewRemoteHost>
        AddRes = UPNP_AddPortMapping(
            urls.controlURL,
            data.first.servicetype,
            ExtPort,
            IntPort,
            lanAddress,
            Desc,
            Proto,
            RemoteHost,
            Lease);
    }

    // 4. Treat “already mapped” as success
    if (AddRes == 718)   // ConflictInMappingEntry
        AddRes = UPNPCOMMAND_SUCCESS;

    // -- existing success / failure handling below ------------------------
    if (AddRes != UPNPCOMMAND_SUCCESS)
    {
        FString Err = ANSI_TO_TCHAR(strupnperror(AddRes));
        UE_LOG(LogTemp, Warning,
            TEXT("MINIUPNP:: Failed to add port %s %s – %s (code %d)"),
            *Protocol, ANSI_TO_TCHAR(ExtPort), *Err, AddRes);
        GEngine->AddOnScreenDebugMessage(
            -1, 15.f, FColor::Red,
            FString::Printf(TEXT("UPnP error (%s): %s"), *Protocol, *Err));
        return false;
    }

    ForwardedPorts.Add(Port);
    UE_LOG(LogTemp, Log, TEXT("MINIUPNP:: Port %d %s mapped OK"), Port, *Protocol);
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
        FString::Printf(TEXT("Port %d %s forwarded"), Port, *Protocol));
    return true;
}

bool UpnpManager::RemovePortMapping(int32 Port, const FString& Protocol)
{
    if (!bIsInitialized)
    {
        FString WarningMsg = FString(TEXT("MINIUPNP:: UPnPManager not initialized. Call Initialize() first."));
        UE_LOG(LogTemp, Warning, TEXT("%s"), *WarningMsg);
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 150.0f, FColor::Red, WarningMsg);
        }
        return false;
    }

    FString PortString = FString::FromInt(Port);

    int deletePortMappingResult = UPNP_DeletePortMapping(
        urls.controlURL,
        data.first.servicetype,
        TCHAR_TO_ANSI(*PortString),
        TCHAR_TO_ANSI(*Protocol),
        nullptr // Remote Host (nullptr for wildcard)
    );

    if (deletePortMappingResult != UPNPCOMMAND_SUCCESS)
    {
        FString ErrorDescription = ANSI_TO_TCHAR(strupnperror(deletePortMappingResult));
        FString ErrorMsg = FString::Printf(TEXT("MINIUPNP:: Failed to delete port mapping: %s"), *ErrorDescription);
        UE_LOG(LogTemp, Warning, TEXT("%s"), *ErrorMsg);
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 150.0f, FColor::Red, FString::Printf(TEXT("Error: %s"), *ErrorDescription));
        }
        return false;
    }

    FString SuccessMessage = FString::Printf(TEXT("MINIUPNP:: Port %d %s mapping removed successfully."), Port, *Protocol);
    UE_LOG(LogTemp, Log, TEXT("%s"), *SuccessMessage);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 150.0f, FColor::Green, SuccessMessage);
    }

    if (deletePortMappingResult == UPNPCOMMAND_SUCCESS)
    {
        ForwardedPorts.Remove(Port);
    }

    return true;
}