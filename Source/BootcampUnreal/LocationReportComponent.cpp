// Fill out your copyright notice in the Description page of Project Settings.


#include "LocationReportComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
ULocationReportComponent::ULocationReportComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULocationReportComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void ULocationReportComponent::ReportLocation()
{
	UE_LOG(LogTemp, Warning, TEXT("Location, %s"), *GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation().ToString());
}

// Called every frame
void ULocationReportComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

