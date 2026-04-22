#include "MZCharacter.h"
#include "MileZero.h"
#include "MileZero/Vehicles/MZVehiclePawn.h"
#include "MileZero/Core/MZPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/CollisionProfile.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

AMZCharacter::AMZCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Third-person camera rig
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Vehicle detection sphere
	VehicleDetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("VehicleDetectionSphere"));
	VehicleDetectionSphere->SetupAttachment(RootComponent);
	VehicleDetectionSphere->SetSphereRadius(300.0f);
	VehicleDetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	VehicleDetectionSphere->SetCollisionObjectType(ECC_Pawn);
	VehicleDetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	VehicleDetectionSphere->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap);

	// Movement defaults
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	GetCharacterMovement()->JumpZVelocity = 420.0f;
}

void AMZCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Bind vehicle detection sphere events
	if (VehicleDetectionSphere)
	{
		VehicleDetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AMZCharacter::OnVehicleDetectionOverlapBegin);
		VehicleDetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AMZCharacter::OnVehicleDetectionOverlapEnd);
	}

	UE_LOG(LogMileZero, Log, TEXT("MZCharacter spawned"));
}

void AMZCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC) return;

	if (IA_Move)
	{
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMZCharacter::HandleMove);
	}
	if (IA_Look)
	{
		EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMZCharacter::HandleLook);
	}
	if (IA_Jump)
	{
		EIC->BindAction(IA_Jump, ETriggerEvent::Started, this, &AMZCharacter::HandleJump);
	}
	if (IA_Interact)
	{
		EIC->BindAction(IA_Interact, ETriggerEvent::Started, this, &AMZCharacter::HandleInteract);
	}
}

void AMZCharacter::HandleMove(const FInputActionValue& Value)
{
	FVector2D MoveInput = Value.Get<FVector2D>();
	const FRotator YawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
	const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(ForwardDir, MoveInput.Y);
	AddMovementInput(RightDir, MoveInput.X);
}

void AMZCharacter::HandleLook(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AMZCharacter::HandleJump()
{
	Jump();
}

void AMZCharacter::HandleInteract()
{
	if (NearbyVehicle)
	{
		EnterVehicle();
	}
}

void AMZCharacter::EnterVehicle()
{
	if (!NearbyVehicle)
	{
		UE_LOG(LogMileZero, Warning, TEXT("No nearby vehicle to enter"));
		return;
	}

	AMZPlayerController* PC = Cast<AMZPlayerController>(GetController());
	if (!PC)
	{
		UE_LOG(LogMileZero, Warning, TEXT("No player controller"));
		return;
	}

	PC->RequestEnterVehicle();
}

void AMZCharacter::OnVehicleDetectionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMZVehiclePawn* Vehicle = Cast<AMZVehiclePawn>(OtherActor);
	if (Vehicle)
	{
		NearbyVehicle = Vehicle;
		UE_LOG(LogMileZero, Log, TEXT("Vehicle detected nearby: %s"), *Vehicle->GetName());
	}
}

void AMZCharacter::OnVehicleDetectionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMZVehiclePawn* Vehicle = Cast<AMZVehiclePawn>(OtherActor);
	if (Vehicle && NearbyVehicle == Vehicle)
	{
		NearbyVehicle = nullptr;
		UE_LOG(LogMileZero, Log, TEXT("Vehicle left detection range"));
	}
}
