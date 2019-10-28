// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BootcampUnrealCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include <BootcampUnreal\LocationReportComponent.h>
#include "Components/PrimitiveComponent.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"


//////////////////////////////////////////////////////////////////////////
// ABootcampUnrealCharacter

ABootcampUnrealCharacter::ABootcampUnrealCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	LocationReport = CreateDefaultSubobject<ULocationReportComponent>(TEXT("LocationReport"));

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ABootcampUnrealCharacter::OnCompHit);

	IsHolding = false;
	IsFirstPerson = false;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABootcampUnrealCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABootcampUnrealCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABootcampUnrealCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABootcampUnrealCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABootcampUnrealCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ABootcampUnrealCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ABootcampUnrealCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ABootcampUnrealCharacter::OnResetVR);

	PlayerInputComponent->BindAction("ReportLocation", IE_Pressed, this, &ABootcampUnrealCharacter::ReportLocation);
	
	PlayerInputComponent->BindAction("Grab", IE_Pressed, this, &ABootcampUnrealCharacter::Grab);

	PlayerInputComponent->BindAction("Camera", IE_Pressed, this, &ABootcampUnrealCharacter::ChangeCamera);
}

void ABootcampUnrealCharacter::ReportLocation()
{

	LocationReport->ReportLocation();
}


void ABootcampUnrealCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ABootcampUnrealCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ABootcampUnrealCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ABootcampUnrealCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABootcampUnrealCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ABootcampUnrealCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABootcampUnrealCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ABootcampUnrealCharacter::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s"), *OtherActor->GetName()));
	}
}

void ABootcampUnrealCharacter::Grab()
{
	if (!IsHolding) {
		
		FVector Start = GetMesh()->GetSocketLocation("middle_01_r");
		FVector ForwardVector = FollowCamera->GetForwardVector();
		FVector End = ((ForwardVector * 600.f) + Start);

		FHitResult Hit;

		FComponentQueryParams DefaultComponentQueryParams;
		FCollisionResponseParams DefaultResponseParam;

		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, DefaultComponentQueryParams, DefaultResponseParam))
		{
			if (Hit.GetActor())
			{
				if (Hit.GetActor()->GetClass()->IsChildOf(AWoodLogs::StaticClass()))
				{
					HoldingWood = Cast<AWoodLogs>(Hit.GetActor());
					if (HoldingWood)
					{
						HoldingWood->GrabOrRelease();
						IsHolding = true;
					}
				}
			}
		}
	}
	else
	{
		HoldingWood->GrabOrRelease();
		IsHolding = false;
		FVector ForwardVector = FollowCamera->GetForwardVector();
		HoldingWood->Mesh->AddImpulse(ForwardVector * 1000 * HoldingWood->Mesh->GetMass());
		HoldingWood = nullptr;
	}
	
}

void ABootcampUnrealCharacter::ChangeCamera()
{
	if (IsFirstPerson)
	{
		CameraBoom->TargetArmLength = 300.0f;
		CameraBoom->SocketOffset = FVector(0);
		this->bUseControllerRotationYaw = false;
		CameraBoom->AttachTo(RootComponent);
	}
	else
	{
		CameraBoom->TargetArmLength = 0;
		CameraBoom->SocketOffset = FVector(0,0,15);
		this->bUseControllerRotationYaw = true;
		CameraBoom->AttachTo(GetMesh(), "head");
	}
	IsFirstPerson = !IsFirstPerson;
}

void ABootcampUnrealCharacter::Tick(float DeltaTime)
{
	if (IsHolding)
	{
		HoldingWood->Mesh->SetWorldLocation(GetMesh()->GetSocketLocation("middle_01_r"));
	}
}

