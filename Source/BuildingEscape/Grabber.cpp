// Copyright Mad Skald Studios

#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Grabber.h"

#define OUT
// Sets default values for this component's properties
UGrabber::UGrabber()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInput();
 
	
}

//Checking for phsyics component
void UGrabber::FindPhysicsHandle()
{
	//UE_LOG(LogTemp, Warning, TEXT("GRABBER ON"));
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("PHYSICS HANDLE MISSING ON OBJECT %s"), *GetOwner()->GetName());
	}


}

//Setup input for Grabber
void UGrabber::SetupInput()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Input Component Found On %s"), *GetOwner()->GetName());

		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);

	}
	else {
		//UE_LOG(LogTemp, Error, TEXT("Input Component MISSING On %s"), *GetOwner()->GetName());
	}
}


FVector UGrabber::GetPlayersReach() const
{
	FVector PlayerViewpointLocation;
	FRotator PlayerViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewpointLocation,
		OUT PlayerViewpointRotation
	);

	//Draw line from player indicating reach
	//UE_LOG(LogTemp, Warning, TEXT("GRAB PRESSED"));
	return PlayerViewpointLocation + PlayerViewpointRotation.Vector() * Reach;
	

	
}

//Grab Functionality
void UGrabber::Grab() 
{
	//Draw line from player indicating reach

	
	//TODO Only raycast when key pressed to check reach
	
	
	//UE_LOG(LogTemp, Warning, TEXT("GRAB PRESSED"));
	FHitResult HitResult = GetFirstPhysicsBodyInReach();

	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	
	AActor* ActorHit = HitResult.GetActor();

	//Try and reach actors with physics body collision channel set
	//if hit, attach physics handle
	//TODO attach handle
	if (ActorHit) 
	{
		if (!PhysicsHandle) { return; }
		
		//UE_LOG(LogTemp, Warning, TEXT("HIT TRUE"));
		PhysicsHandle->GrabComponentAtLocation
		(
			ComponentToGrab,
			NAME_None,
			GetPlayersReach()
		);
	}
	

}


//Release functionality
void UGrabber::Release() 
{
	if (!PhysicsHandle) {return;}
	
	//UE_LOG(LogTemp, Warning, TEXT("GRAB RELEASED"));
	//TODO Remove/release the physics object currently grabbed
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!PhysicsHandle) { return; }
		
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(GetPlayersReach());
	}


	//GetPlayers Viewpoint

}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	//UE_LOG(LogTemp, Warning, TEXT("func called"));
	FHitResult Hit;
	//raycast out arm distance (reach)
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayersWorldPos(),
		GetPlayersReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	AActor* ActorHit = Hit.GetActor();

	if (ActorHit)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Line trace hit %s"), *(ActorHit->GetName()));
	}
	else {
		//UE_LOG(LogTemp, Warning, TEXT("Nothing hit"));
	}

	//check raycast hits

	return Hit;
}

//Get Players World Position
FVector UGrabber::GetPlayersWorldPos() const
{
	FVector PlayerViewpointLocation;
	FRotator PlayerViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewpointLocation,
		OUT PlayerViewpointRotation
	);

	return PlayerViewpointLocation;
	


}

