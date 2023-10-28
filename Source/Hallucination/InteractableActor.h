// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class HALLUCINATION_API AInteractableActor : public AActor
{
	GENERATED_BODY()

	//UPROPERTY(EditAnywhere)
	//USceneComponent* DefaultRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* staticMesh;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hit, meta = (AllowPrivateAccess = "true"))
	bool CanHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hit, meta = (AllowPrivateAccess = "true"))
	bool OnHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hit, meta = (AllowPrivateAccess = "true"))
	FTimerHandle hitTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	USoundBase* SB_Clash;
	
public:	
	// Sets default values for this actor's properties
	AInteractableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HitMesh(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
