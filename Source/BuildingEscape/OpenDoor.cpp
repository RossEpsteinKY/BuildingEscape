// Copyright Mad Skald Studios


#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/PlayerController.h"
#include "OpenDoor.h"
#include "GameFramework/Actor.h"


// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw;

	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s OPEN DOOR COMPONENT ON OBJECT, NO PRESSUREPLATE SET!"), *GetOwner()->GetName());
	}
	FindAudioComponent();
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	if (!AudioComponent) { return; }
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *GetOwner()->GetActorRotation().ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Yaw is: %f"), GetOwner()->GetActorRotation().Yaw);
	
	// Door Open Math
	//if (PressurePlate && PressurePlate->IsOverlappingActor(ActorThatOpens)) 
	if(TotalMassOfActors() > MassToOpen)
	{

		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else{
		if (GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay)
		{
		
			CloseDoor(DeltaTime);
		}
	}
	
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	CurrentYaw = FMath::Lerp(CurrentYaw, OpenAngle, DeltaTime * DoorOpenSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	CloseDoorSound = false;
	if (!AudioComponent) { return; }

	if (!OpenDoorSound)
	{
		AudioComponent->Play();
		OpenDoorSound = true;
	}
	
	
	
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	CurrentYaw = FMath::Lerp(CurrentYaw, InitialYaw, DeltaTime * DoorCloseSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
	if (!AudioComponent) { return; }
	OpenDoorSound = false;
	if (!CloseDoorSound)
	{
		AudioComponent->Play();
		CloseDoorSound = true;
	}
}

float UOpenDoor::TotalMassOfActors() const 
{
	float TotalMass = 0.f;

	if (!PressurePlate) {return TotalMass;}
	//find all overlapping actors
	TArray<AActor*> OverlappingActors;
	PressurePlate->GetOverlappingActors(OverlappingActors);


	//add up actor masses

	for (AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}

	return TotalMass;
		
}