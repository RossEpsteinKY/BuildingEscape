// Copyright Mad Skald Studios

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();



public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//Grab and Release Functions
	void Grab();
	void Release();
	void FindPhysicsHandle();
	void SetupInput();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	
	//Reach for Player Grab
	UPROPERTY(EditAnywhere)
	float Reach = 100.f;

	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	UInputComponent* InputComponent = nullptr;

	// Return First Actor In Reach With Physics Body
	FHitResult GetFirstPhysicsBodyInReach() const;

	//Return Line Trace End
	FVector GetPlayersReach() const;

	FVector GetPlayersWorldPos() const;
		
};
