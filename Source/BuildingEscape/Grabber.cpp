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
	if (PhysicsHandle)
	{
		//physics found
	}

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
		UE_LOG(LogTemp, Warning, TEXT("Input Component Found On %s"), *GetOwner()->GetName());

		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);

	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Input Component MISSING On %s"), *GetOwner()->GetName());
	}
}

//Grab Functionality
void UGrabber::Grab() 
{
	UE_LOG(LogTemp, Warning, TEXT("GRAB PRESSED"));

	FVector PlayerViewpointLocation;
	FRotator PlayerViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewpointLocation,
		OUT PlayerViewpointRotation
	);

	//Draw line from player indicating reach

	FVector LineTraceEnd = PlayerViewpointLocation + PlayerViewpointRotation.Vector() * Reach;
	
	//TODO Only raycast when key pressed to check reach
	
	
	FHitResult HitResult = GetFirstPhysicBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();


	//Try and reach actors with physics body collision channel set
	//if hit, attach physics handle
	//TODO attach handle
	if (HitResult.GetActor()) 
	{
		PhysicsHandle->GrabComponentAtLocation
		(
			ComponentToGrab,
			NAME_None,
			LineTraceEnd
		);
	}
}

//Release functionality
void UGrabber::Release() 
{
	UE_LOG(LogTemp, Warning, TEXT("GRAB RELEASED"));
	//TODO Remove/release the physics object currently grabbed
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	FVector PlayerViewpointLocation;
	FRotator PlayerViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewpointLocation,
		OUT PlayerViewpointRotation
	);

	//Draw line from player indicating reach

	FVector LineTraceEnd = PlayerViewpointLocation + PlayerViewpointRotation.Vector() * Reach;
	

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//	GetFirstPhysicBodyInReach();
	
	//If physics handle is attached
		//Move object we are holding

	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}


	//GetPlayers Viewpoint

}

FHitResult UGrabber::GetFirstPhysicBodyInReach() const
{
	FVector PlayerViewpointLocation;
	FRotator PlayerViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewpointLocation,
		OUT PlayerViewpointRotation
	);

	//Draw line from player indicating reach

	FVector LineTraceEnd = PlayerViewpointLocation + PlayerViewpointRotation.Vector() * Reach;

	//Debug Draw Line

	//DrawDebugLine(
	//	GetWorld(),
	//	PlayerViewpointLocation,
	//	LineTraceEnd,
	//	FColor(0, 255, 0),
	//	false,
	//	0.f,
	//	0,
	//	5.f
	//);


	FHitResult Hit;
	//raycast out arm distance (reach)
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewpointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	AActor* ActorHit = Hit.GetActor();

	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit %s"), *(ActorHit->GetName()));
	}

	//check raycast hits

	return Hit;


}