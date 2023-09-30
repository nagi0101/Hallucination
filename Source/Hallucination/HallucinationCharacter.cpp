// Copyright Epic Games, Inc. All Rights Reserved.

#include "HallucinationCharacter.h"
#include "HallucinationProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


//////////////////////////////////////////////////////////////////////////
// AHallucinationCharacter

AHallucinationCharacter::AHallucinationCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	//PhysicsHandle
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

	// Movement
	UCharacterMovementComponent* movement = GetCharacterMovement();
	check(movement);
	WalkSpeed = 300.f;
	RunSpeed = 600.f;
	movement->MaxWalkSpeed = WalkSpeed;
	MaxStemina = 100.f;
	Stemina = MaxStemina;
	IsExhaused = false;
	IsRunning = false;
	SteminaRecoveryRate = 10.f;
	SteminaConsumptionRun = 10.f;
	SteminaConsumptionBreath = 5.f;
	SteminaRecoveryThreshold = 20.f;
	// Crouch
	movement->MaxWalkSpeedCrouched = WalkSpeed / 2.f;
	movement->GetNavAgentPropertiesRef().bCanCrouch = true;
	movement->SetCrouchedHalfHeight(GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 2.f);

	// Camera Shake
	CS_Idle = nullptr;
	CS_Run = nullptr;
	CS_Walk = nullptr;

	// Post Process
	M_Vinyette = nullptr;
	MD_Vinyette = nullptr;

	// Breath
	IsHoldingBreath = false;
	SB_Inhale = nullptr;
	SB_Exhale = nullptr;
	SB_ExhaleStrong = nullptr;

	//Interact
	IsGrabbing = false;
	onPushingAndPulling = false;
}

void AHallucinationCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// PostProcess
	SetPostProcessMaterialInstance(M_Vinyette, MD_Vinyette);
	SetPostProcessParameter();
}

void AHallucinationCharacter::SetCameraShake(FVector velocity)
{
	double speed = velocity.Length();
	APlayerController* controller = GetWorld()->GetFirstPlayerController();

	check(controller != nullptr);

	TSubclassOf<UCameraShakeBase> cameraShake;
	if (speed == 0.f) {
		cameraShake = CS_Idle;
	}
	else if (speed <= WalkSpeed) {
		cameraShake = CS_Walk;
	}
	else {
		cameraShake = CS_Run;
	}	
	controller->ClientPlayCameraShake(cameraShake, 1.0f);
}

void AHallucinationCharacter::CheckStemina(float deltaTime)
{
	float steminaConsumption = 
		IsRunning * SteminaConsumptionRun + 
		IsHoldingBreath	* SteminaConsumptionBreath;
	float deltaStemina = deltaTime * (steminaConsumption > 0.f ? -steminaConsumption : SteminaRecoveryRate);
	Stemina = FMath::Clamp(Stemina + deltaStemina, 0.f, MaxStemina);

	if (IsExhaused)
	{
		if (Stemina >= SteminaRecoveryThreshold)
		{
			IsExhaused = false;
		}
	}
	else
	{
		if (Stemina <= 0.f)
		{
			IsExhaused = true;
			EndSprint();
			EndHoldBreath();
		}
	}
}

void AHallucinationCharacter::StartSprint()
{
	UCharacterMovementComponent* movement = GetCharacterMovement();

	check(movement);
	if (IsExhaused == false)
	{
		IsRunning = true;
		movement->MaxWalkSpeed = RunSpeed;
	}
}

void AHallucinationCharacter::EndSprint()
{
	UCharacterMovementComponent* movement = GetCharacterMovement();
	check(movement);
	movement->MaxWalkSpeed = WalkSpeed;
	IsRunning = false;
}

void AHallucinationCharacter::StartHoldBreath()
{
	if (IsHoldingBreath || IsExhaused) return;
	
	UWorld* world = GetWorld();
	check(world);

	IsHoldingBreath = true;
	UGameplayStatics::PlaySound2D(world, SB_Inhale);
}

void AHallucinationCharacter::EndHoldBreath()
{
	if (!IsHoldingBreath) return;
	
	UWorld* world = GetWorld();
	check(world);

	IsHoldingBreath = false;
	USoundBase* soundToPlay = IsExhaused ? SB_ExhaleStrong : SB_Exhale;
	UGameplayStatics::PlaySound2D(world, soundToPlay);
}

void AHallucinationCharacter::SetPostProcessParameter()
{
	float steminaRate = Stemina / MaxStemina;
	
	float vinyetteStart = 0.2f * steminaRate + 0.1f * (1 - steminaRate);
	float vinyetteEnd = 1.f * steminaRate + 0.8f * (1 - steminaRate);
	float vinyetteMax = 0.8f * steminaRate + 0.9f * (1 - steminaRate);

	MD_Vinyette->SetScalarParameterValue("VinyetteStart", vinyetteStart);
	MD_Vinyette->SetScalarParameterValue("VinyetteEnd", vinyetteEnd);
	MD_Vinyette->SetScalarParameterValue("VinyetteMax", vinyetteMax);
}

void AHallucinationCharacter::SetPostProcessMaterialInstance(UMaterialInterface* &Material, UMaterialInstanceDynamic* &DynamicMaterial)
{
	DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
	FirstPersonCameraComponent->AddOrUpdateBlendable(DynamicMaterial);
}

void AHallucinationCharacter::Interact() {
	if (onPushingAndPulling) {
		onPushingAndPulling = false;
		interactedObject = NULL;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("End Pushing and Pulling"));
		return;
	}
	if (IsGrabbing) {
		Putdown();
		return;
	}
	FVector start = FirstPersonCameraComponent->GetComponentLocation();
	FVector cameraForwardVector = FirstPersonCameraComponent->GetForwardVector() * 500.0f;
	FVector end = start + cameraForwardVector;
	FHitResult hit;
	FCollisionQueryParams traceParams;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Start Interact"));
	if (GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, traceParams)) {
		AActor* hitActor = hit.GetActor();
		if (hitActor->ActorHasTag("Moveable") && !IsGrabbing) {
			onPushingAndPulling = true;
			interactedObject = hit.GetActor();
			disToObject = FVector2D(interactedObject->GetActorLocation() - GetActorLocation());
		}
		else if (hitActor->ActorHasTag("Pickable") && !onPushingAndPulling) {
			Pickup(hit);
		}
	}
}

void AHallucinationCharacter::Pickup(FHitResult hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("On Pickup"));
	UPrimitiveComponent* hitComponent = hit.GetComponent();
	FVector hitLocation = hitComponent->GetComponentLocation();
	hitComponent->WakeRigidBody();
	PhysicsHandle->GrabComponentAtLocation(hitComponent, "None", hitLocation);
	IsGrabbing = true;
}

void AHallucinationCharacter::Putdown() {
	PhysicsHandle->ReleaseComponent();
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("End Pickup"));
	IsGrabbing = false;
}

void AHallucinationCharacter::Throw() {
	if (!PhysicsHandle) return;
	UPrimitiveComponent* grabbedComp = PhysicsHandle->GetGrabbedComponent();
	if (!grabbedComp) return;
	FVector cameraForwardVector = FirstPersonCameraComponent->GetForwardVector();

	grabbedComp->AddImpulse(cameraForwardVector * 1000.0f, NAME_None, true);
	IsGrabbing = false;
	PhysicsHandle->ReleaseComponent();
}

void AHallucinationCharacter::PushAndPull(FVector direction, float scale) {
	//play anim
	
	if (interactedObject != NULL) {
		//debug
		FString interactedObjectName = GetDebugName(interactedObject);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("On Pushing and Pulling"));
		//
		//when you trigger IA_ForwardBackWard, it not only move player but also hit object
		AddMovementInput(direction, scale);

		FVector playerLocation = GetActorLocation();
		FVector objectLocation = interactedObject->GetActorLocation();
		FVector forward = GetActorForwardVector();
		FVector2D newLocation = FVector2D(playerLocation.X + disToObject.X, playerLocation.Y + disToObject.Y);
		UE_LOG(LogTemp, Log, TEXT("playerLocation : %s"), *playerLocation.ToString());
		UE_LOG(LogTemp, Log, TEXT("objectLocation : %s"), *objectLocation.ToString());
		UE_LOG(LogTemp, Log, TEXT("dis : %s"), *disToObject.ToString());
		UE_LOG(LogTemp, Log, TEXT("newLocation : %s"), *newLocation.ToString());
		interactedObject->SetActorLocation(FVector(newLocation.X, newLocation.Y, objectLocation.Z));
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AHallucinationCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	//// Set up gameplay key bindings
	//check(PlayerInputComponent);

	//// Bind jump events
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//// Bind fire event
	//PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &AHallucinationCharacter::OnPrimaryAction);

	//// Enable touchscreen input
	//EnableTouchscreenMovement(PlayerInputComponent);

	//// Bind movement events
	//PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AHallucinationCharacter::MoveForward);
	//PlayerInputComponent->BindAxis("Move Right / Left", this, &AHallucinationCharacter::MoveRight);

	//// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	//// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	//// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	//PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	//PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	//PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AHallucinationCharacter::TurnAtRate);
	//PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AHallucinationCharacter::LookUpAtRate);
}

void AHallucinationCharacter::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
}

void AHallucinationCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnPrimaryAction();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AHallucinationCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void AHallucinationCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AHallucinationCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AHallucinationCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AHallucinationCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

bool AHallucinationCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AHallucinationCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AHallucinationCharacter::EndTouch);

		return true;
	}
	
	return false;
}
