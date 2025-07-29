// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "miniupnpc.h"
/**
 * 
 */
class V2V_API UpnpManager
{
public:
	UpnpManager();
	~UpnpManager();
	bool Initialize(); // Perform UPnP discovery
	bool AddPortMapping(int32 Port, const FString& Protocol);
	bool RemovePortMapping(int32 Port, const FString& Protocol);
	static bool MappingPointsToMe(const char* Ctrl, const char* Type,
		const char* PortStr, const char* Proto,
		const char* MyLAN);

private:
	UPNPUrls urls;
	IGDdatas data;
	char lanAddress[16]; // Local IP address
	char wanAddress[16]; // External WAN IP address

	bool bIsInitialized; // Flag to check if discovery was successful

	TSet<int32> ForwardedPorts;
};
