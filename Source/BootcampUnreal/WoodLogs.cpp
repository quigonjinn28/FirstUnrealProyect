// Fill out your copyright notice in the Description page of Project Settings.


#include "WoodLogs.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AWoodLogs::AWoodLogs()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh(TEXT("/Game/ThirdPersonCPP/Blueprints/WoodSpawner/Cylinder.Cylinder"));
	if (FoundMesh.Succeeded())
	{
		Mesh->SetStaticMesh(FoundMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("/Game/ThirdPersonCPP/Materials/M_Wood_Walnut.M_Wood_Walnut"));
	if (FoundMaterial.Succeeded())
	{
		Mesh->SetMaterial(0, FoundMaterial.Object);
	}
	
	Mesh->SetWorldScale3D(FVector(0.2, 0.2, 0.4));
	Mesh->SetWorldRotation(FRotator(66.873512, 89.663040, -90.232002));
	Mesh->SetSimulatePhysics(true);


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

