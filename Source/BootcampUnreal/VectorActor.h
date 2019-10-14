// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VectorActor.generated.h"

USTRUCT()
struct FLine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		FVector begin;
	UPROPERTY(EditAnywhere)
		FVector end;

	FLine()
	{
		begin = FVector(0);
		end = FVector(0);
	}
	
	FLine(FVector x, FVector y)
	{
		begin = x;
		end = y;
	}
};

UCLASS()
class BOOTCAMPUNREAL_API AVectorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVectorActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
		USceneComponent* Root;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
		FLine Line1;

	UPROPERTY(EditAnywhere)
		FLine Line2;

	UPROPERTY(EditAnywhere)
		AActor* Player;

private:
	//To make the vectors visible
	FVector OffSet;
};
