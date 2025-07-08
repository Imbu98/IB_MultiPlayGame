#include "IB_RPGGameMode.h"

#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Networking.h"

#include "../IB_GameInstanceSubSystem.h"
#include "IB_RPGPlayerController.h"


void AIB_RPGGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}

UIB_CharacterClassInfo* AIB_RPGGameMode::GetCharacterClassDefaultInfo() const
{
	return ClassDefaults;
}

UProjectile_Info* AIB_RPGGameMode::GetProjectileInfo() const
{
	return ProjectileInfo;
}

UWeapon_Info* AIB_RPGGameMode::GetWeaponInfo() const
{
	return WeaponInfo;
}

UArmorInfo* AIB_RPGGameMode::GetArmorInfo() const
{
	return ArmorInfo;
}

void AIB_RPGGameMode::HandleDungeonClear()
{
	FString Msg = FString::Printf(TEXT("%d"), GameModePortNumber);
	FTCHARToUTF8 Convert(*Msg);
	const uint8* Data = (const uint8*)Convert.Get();

	FIPv4Address LobbyAddr;
	FIPv4Address::Parse(TEXT("192.168.0.176"), LobbyAddr); // Replace with actual lobby server IP

	TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	Addr->SetIp(LobbyAddr.Value);
	Addr->SetPort(6000);

	FSocket* SendSocket = FUdpSocketBuilder(TEXT("DungeonNotifySender"))
		.AsReusable()
		.WithBroadcast()
		.WithSendBufferSize(2 * 1024);

	int32 BytesSent = 0;
	SendSocket->SendTo(Data, Msg.Len(), BytesSent, *Addr);
	SendSocket->Close();
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SendSocket);

	UE_LOG(LogTemp, Log, TEXT("Sent shutdown notification to lobby for port %d"), GameModePortNumber);
}

void AIB_RPGGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	// APlayerController* PC = Cast<APlayerController>(Exiting);
	// if (PC)
	// {
	// 	ActiveControllers.Remove(PC);
	// 	UE_LOG(LogTemp, Log, TEXT("Removed controller. Remaining: %d"), ActiveControllers.Num());
	// 	UE_LOG(LogTemp, Log, TEXT("To RemovePort: %d"), GameModePortNumber);
	//
	// 	if (ActiveControllers.Num() == 0)
	// 	{
	// 		HandleDungeonClear();
	// 	}
	// }
}

void AIB_RPGGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	APlayerController* PC = Cast<APlayerController>(NewPlayer);
	if (PC)
	{
		ActiveControllers.Add(NewPlayer);
	}
}
