// Fill out your copyright notice in the Description page of Project Settings.


#include "WoodLogs.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AWoodLogs::AWoodLogs()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	bHolding = false;
	bGravity = true;
}

// Called when the game starts or when spawned
void AWoodLogs::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AWoodLogs::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWoodLogs::GrabOrRelease()
{
	bHolding = !bHolding;
	bGravity = !bGravity;
	Mesh->SetEnableGravity(bGravity);
	Mesh->SetSimulatePhysics(bHolding ? false : true);
	Mesh->SetCollisionEnabled(bHolding ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);

}

