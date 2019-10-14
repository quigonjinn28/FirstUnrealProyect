// Fill out your copyright notice in the Description page of Project Settings.


#include "VectorActor.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Math/Vector.h"
#include "Engine/Engine.h"

// Sets default values
AVectorActor::AVectorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->AttachTo(Root);

}

// Called when the game starts or when spawned
void AVectorActor::BeginPlay()
{
	Super::BeginPlay();

	OffSet = FVector(1300, 0, 330);
	
	DrawDebugLine(
		GetWorld(),
		Line1.begin + OffSet,
		Line1.end + OffSet,
		FColor(255, 0, 0),
		true, -1, 0,
		12.333
	);

	DrawDebugLine(
		GetWorld(),
		Line2.begin+ OffSet,
		Line2.end + OffSet,
		FColor(0, 255, 0),
		true, -1, 0,
		12.333
	);
	
	//To make more visible because I tried a example where x value was the same one
	FLine result = FLine(Line1.begin + Line2.begin, Line1.end + Line2.end);
	result.begin += OffSet;
	result.end += OffSet;

	DrawDebugLine(
		GetWorld(),
		result.begin,
		result.end,
		FColor(0, 0, 255),
		true, -1, 0,
		12.333
	);
	//To make more visible because I tried a example where x value was the same one

	FLine orthogonal = FLine(FVector::CrossProduct(Line1.begin, Line2.begin), FVector::CrossProduct(Line1.end, Line2.end));
	orthogonal.begin += OffSet;
	orthogonal.end += OffSet;
	orthogonal.begin += OffSet;
	orthogonal.end += OffSet;


		DrawDebugLine(
			GetWorld(),
			orthogonal.begin,
			orthogonal.end,
			FColor(0, 0, 0),
			true, -1, 0,
			18
		);
	
}

// Called every frame
void AVectorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (FVector::DotProduct(Player->GetActorForwardVector() , GetActorForwardVector()) > 0.95)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Stop copying me"));
	}

}

