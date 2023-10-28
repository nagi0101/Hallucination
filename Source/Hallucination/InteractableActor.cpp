// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActor.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"

// Sets default values
AInteractableActor::AInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	//SetRootComponent(DefaultRoot);

	staticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	staticMesh->OnComponentHit.AddDynamic(this, &AInteractableActor::HitMesh);
	SetRootComponent(staticMesh);
	//staticMesh->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);


	OnHit = false;
	CanHit = true;

	SB_Clash= nullptr;
}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	staticMesh->OnComponentHit.AddDynamic(this, &AInteractableActor::HitMesh);

	GetWorld()->GetTimerManager().ClearTimer(hitTimer);
}

// Called every frame
void AInteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractableActor::HitMesh(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

	if (CanHit) {
		if (OnHit) {
			if (GetWorld()->GetTimerManager().GetTimerRemaining(hitTimer) <= 0) {
				GetWorld()->GetTimerManager().SetTimer(hitTimer, [&]()
					{
						GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Hit Clear"));
						OnHit = false;
						GetWorld()->GetTimerManager().ClearTimer(hitTimer);
					}, 0.3f, false);
			}
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("On Component HIt"));
			FVector meshLocation = staticMesh->GetComponentLocation();
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("meshLocation : %s"),*meshLocation.ToString()));
			UGameplayStatics::PlaySoundAtLocation(this, SB_Clash, meshLocation);
			UAISense_Hearing::ReportNoiseEvent(GetWorld(), meshLocation, 1.0f, this, 0.0f, "");
			OnHit = true;
		}
	}
}
