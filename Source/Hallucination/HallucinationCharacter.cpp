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
#include <Kismet/KismetMathLibrary.h>
#include "DynamicGravityCharacterComponent.h"
#include "InteractableObjectInterface.h"

#define D(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT(x));}
//////////////////////////////////////////////////////////////////////////
// AHallucinationCharacter

AHallucinationCharacter::AHallucinationCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Setup springArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetCapsuleComponent());
	SpringArm->TargetArmLength = 0.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagSpeed = 0.f;
	SpringArm->CameraRotationLagSpeed = 15.f;

	controllerPitchMin = -45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(SpringArm);
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = false;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// PhysicsHandle
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

	// Interaction Spot
	InteractionSpot = CreateDefaultSubobject<UArrowComponent>(TEXT("InteractionSpot"));
	InteractionSpot->SetupAttachment(GetCapsuleComponent());

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
	InteractDistance = 250.0f;
	OnPushingAndPulling = false;

	//Skill
	canControlGravitiy = false;

	// HP
	MaxHP = 100.f;
	HP = MaxHP;
	HPRecoveryRate = 20.f;
	HPRecoveryCooltime = 5.f;
	LastDamaged = 0.0f;
	isDead = false;
}

void AHallucinationCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	SetPostProcessMaterialInstance(M_Blood, &MD_Blood);
	SetPostProcessMaterialInstance(M_Vinyette, &MD_Vinyette);

	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMin = controllerPitchMin;
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
	auto vinyetteProperties = TArray<FDynamicMaterialScalarProperty>
	{
		{"VinyetteStart", FMath::Lerp(0.1f, 0.2f, steminaRate)},
		{"VinyetteEnd", FMath::Lerp(0.8f, 1.f, steminaRate)},
		{"VinyetteMax", FMath::Lerp(0.9f, 0.8f, steminaRate)}
	};
	SetPostProcessScalarParameters(MD_Vinyette, vinyetteProperties);

	float HPRate = HP / MaxHP;
	auto bloodProperties = TArray<FDynamicMaterialScalarProperty>
	{
		{"BloodStart", FMath::Lerp(0.f, 1.f, HPRate)},
		{"PulseFrequency", FMath::Lerp(2.f, 0.1f, HPRate)},
		{"PulseMax", FMath::Lerp(0.5f, 0.0f, HPRate)},
	};
	SetPostProcessScalarParameters(MD_Blood, bloodProperties);
}

void AHallucinationCharacter::SetPostProcessScalarParameters(UMaterialInstanceDynamic*& DynamicMaterial, TArray<FDynamicMaterialScalarProperty>& PropertiesInfo)
{
	for (auto propertyInfo : PropertiesInfo)
	{
		DynamicMaterial->SetScalarParameterValue(propertyInfo.Name, propertyInfo.Value);
	}
}

void AHallucinationCharacter::SetPostProcessMaterialInstance(UMaterialInterface*& Material, UMaterialInstanceDynamic** DynamicMaterialOut, float weight)
{
	*DynamicMaterialOut = UMaterialInstanceDynamic::Create(Material, this);
	FirstPersonCameraComponent->AddOrUpdateBlendable(*DynamicMaterialOut, weight);
}

void AHallucinationCharacter::Damage_Implementation(float damage)
{
	UWorld* world = GetWorld();
	check(world);

	HP = FMath::Max(0.f, HP - damage);
	LastDamaged = world->TimeSeconds;
}

void AHallucinationCharacter::CheckHP(float deltaTime)
{
	UWorld* world = GetWorld();
	check(world);

	bool recoverHPFlag = HP < MaxHP && !isDead && world->TimeSince(LastDamaged) >= HPRecoveryCooltime;
	if (recoverHPFlag)
	{
		HP = FMath::Clamp(HP + HPRecoveryRate * deltaTime, 0.f, MaxHP);
	}

	if (!isDead && HP <= 0.f)
	{
		Die();
	}
}

void AHallucinationCharacter::Die()
{
	SpringArm->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	SpringArm->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = false;
	FirstPersonCameraComponent->PostProcessSettings.ColorGradingIntensity = 1.0f;
	FirstPersonCameraComponent->PostProcessSettings.ColorSaturation = FVector4(0.5f, 0.5f, 0.5f, 1.0f);
	FirstPersonCameraComponent->PostProcessSettings.ColorContrast= FVector4(2.0f, 2.0f, 2.0f, 1.0f);

	isDead = true;
}

void AHallucinationCharacter::Revive()
{
	bUseControllerRotationYaw = true;
	SpringArm->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->PostProcessSettings.ColorGradingIntensity = 0.0f;
	FirstPersonCameraComponent->PostProcessSettings.ColorSaturation = FVector4(1.f, 1.f, 1.f, 1.f);
	FirstPersonCameraComponent->PostProcessSettings.ColorContrast = FVector4(1.0f, 1.0f, 1.0f, 1.0f);

	isDead = false;
	HP = MaxHP;
	Stemina = MaxStemina;
}

void AHallucinationCharacter::Interact() {
	if (OnPushingAndPulling) {
		OnPushingAndPulling = false;
		//interactedObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		//interactedComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		PhysicsHandle->ReleaseComponent();

		interactedObject = NULL;
		interactedComp = NULL;

		//PlayAnimMontage(DragEndMontage);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("End Pushing and Pulling"));
		return;
	}
	if (IsGrabbing) {
		Putdown();
		return;
	}
	FVector start = FirstPersonCameraComponent->GetComponentLocation();
	UDynamicGravityCharacterComponent* gravityComponent = Cast<UDynamicGravityCharacterComponent>(GetComponentByClass(UDynamicGravityCharacterComponent::StaticClass()));
	FVector tempForward = gravityComponent->GetGravityRotatedControllForward();
	if (gravityComponent->GetGravityDirection().Z >= 0.0f) {
		tempForward.X *= -1;
		tempForward.Y *= -1;
	}
	FVector cameraForwardVector = tempForward * InteractDistance;
	FVector end = start + cameraForwardVector;
	DrawDebugLine(
		GetWorld(),
		start,
		end,
		FColor(0, 255, 0),
		false, 1.0f, 0,
		12.333
	);
	FHitResult hit;
	FCollisionQueryParams traceParams;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Start Interact"));
	if (GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, traceParams)) {
		//DrawDebugLine(
		//	GetWorld(),
		//	start,
		//	hit.Location,
		//	FColor(255, 0, 0),
		//	false, 1.0f, 0,
		//	12.333
		//);
		interactedObject = hit.GetActor();
		interactedComp = hit.GetComponent();
		FString hitActor = GetDebugName(interactedObject);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("Hit actor: %s"), *hitActor));
		if (interactedObject->ActorHasTag("Moveable") && !IsGrabbing) {
			UnCrouch();
			//PlayAnimMontage(DragStartMontage);

						//GetWorld()->GetFirstPlayerController()->GetCharacter()->GetActorLocation();

			FVector playerLocation = RootComponent->GetComponentLocation();
			FVector objectLocation = interactedComp->GetComponentLocation();
			
			float xDist = FMath::Abs(playerLocation.X - objectLocation.X);
			float yDist = FMath::Abs(playerLocation.Y - objectLocation.Y);
			FVector dir = (playerLocation - objectLocation);
			dir.Normalize();
			FVector newLocation;
			if (xDist > yDist) {
				newLocation = FVector(objectLocation.X + dir.X * InteractDistance / 1.5, objectLocation.Y, playerLocation.Z);
			}
			else {
				newLocation = FVector(objectLocation.X, objectLocation.Y + dir.Y * InteractDistance / 1.5, playerLocation.Z);
			}

			FRotator newRotation = UKismetMathLibrary::FindLookAtRotation(newLocation, objectLocation);
			//if (gravityComponent->GetGravityDirection().Z >= 0.0f) {
			//	newRotation.Roll *= -1;
			//}
			RootComponent->SetWorldLocation(newLocation);
			GetWorld()->GetFirstPlayerController()->SetControlRotation(newRotation);

			FVector hitLocation = interactedComp->GetComponentLocation();
			interactedComp->WakeRigidBody();
			interactedComp->SetWorldLocation(playerLocation + GetActorForwardVector() * InteractDistance);
			PhysicsHandle->GrabComponentAtLocation(interactedComp, "None", hitLocation);

			//interactedComp->SetWorldLocation(playerLocation + GetActorForwardVector() * InteractDistance);

			OnPushingAndPulling = true;
			//interactedComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
			//interactedComp->AttachToComponent(InteractionSpot, FAttachmentTransformRules::KeepWorldTransform);

			//Debug
			//FString name = GetDebugName(interactedObject->GetAttachParentActor());
			//D(name.);
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("Attachd to : %s"),*name));
			//UE_LOG(LogTemp, Log, TEXT("attached to : %s"), *name);
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Start Pushing and Pulling"));
		}
		else if (interactedObject->ActorHasTag("Pickable") && !OnPushingAndPulling) {
			Pickup();
		}
		else if (interactedObject->ActorHasTag("Gravity") && !OnPushingAndPulling && !IsGrabbing) {
			canControlGravitiy = true;
			interactedObject->Destroy();
		}
		else if (interactedObject->ActorHasTag("Usable") && !OnPushingAndPulling && !IsGrabbing) {
			interactedObject->Destroy();
			//SkillToSmaller();
		}
		else if (interactedObject->GetClass()->ImplementsInterface(UInteractableObjectInterface::StaticClass())) {
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Interact Object"));
			// if you use only blueprint for this interface than cast<> will always return nullptr
			// auto* object = Cast<IInteractableObjectInterface>(hitActor);
			IInteractableObjectInterface::Execute_InteractThis(interactedObject);
		}
	}
}

void AHallucinationCharacter::Pickup()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("On Pickup"));
	FVector hitLocation = interactedComp->GetComponentLocation();
	interactedComp->WakeRigidBody();
	PhysicsHandle->GrabComponentAtLocation(interactedComp, "None", hitLocation);
	IsGrabbing = true;

	USoundBase* sound= SB_PickUp;
	//AInteractableActor
	//(InteractableA)
	UWorld* world = GetWorld();
	UGameplayStatics::PlaySound2D(world, sound);
}

void AHallucinationCharacter::Putdown() {
	PhysicsHandle->ReleaseComponent();
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("End Pickup"));
	IsGrabbing = false;

	USoundBase* sound = SB_Putdown;
	UWorld* world = GetWorld();
	UGameplayStatics::PlaySound2D(world, sound);
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

void AHallucinationCharacter::PushAndPull(FVector newLocation) {
	//play anim
	
	if (interactedObject != NULL) {
		//debug
		//FString interactedObjectName = GetDebugName(interactedObject);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, newLocation.ToString());
		PhysicsHandle->SetTargetLocation(FVector(newLocation.X, newLocation.Y, interactedComp->GetComponentLocation().Z));
		//interactedComp->SetWorldLocation(newLocation, true,nullptr,ETeleportType::None);
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("On Pushing and Pulling"));
		//FVector newLocation =
		//	PhysicsHandle->SetTargetLocation()
		//FVector currentLocation = GetActorLocation();
		//interactedObject->SetActorLocation(currentLocation + direction * scale, true);
		//ddMovementInput(direction, scale);
		//FVector currentLocation = GetActorLocation();
		//SetActorLocation(currentLocation + direction * scale, true);
		//UE_LOG(LogTemp, Log, TEXT("playerLocation : %s"), *currentLocation.ToString());
		//FVector playerLocation = GetActorLocation();
		//FVector objectLocation = interactedObject->GetActorLocation();
		////FVector forward = GetActorForwardVector();
		//float forwardX = FMath::RoundToFloat(GetActorForwardVector().X);
		//float forwardY = FMath::RoundToFloat(GetActorForwardVector().Y);
		//float forwardZ = FMath::RoundToFloat(GetActorForwardVector().Z);
		//FVector forward = FVector(forwardX, forwardY, forwardZ);
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, forward.ToString());
		//FVector2D newLocation = FVector2D(playerLocation.X + disToObject.X + 1.0f, playerLocation.Y + disToObject.Y + 1.0f);
		//UE_LOG(LogTemp, Log, TEXT("playerLocation : %s"), *playerLocation.ToString());
		//UE_LOG(LogTemp, Log, TEXT("objectLocation : %s"), *objectLocation.ToString());
		//UE_LOG(LogTemp, Log, TEXT("dis : %s"), *disToObject.ToString());
		//UE_LOG(LogTemp, Log, TEXT("newLocation : %s"), *newLocation.ToString());
		//interactedObject->SetActorLocation(FVector(newLocation.X, newLocation.Y, objectLocation.Z),true);


		USoundBase* sound = SB_Drag;
		UWorld* world = GetWorld();
		UGameplayStatics::PlaySound2D(world, sound);
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
