#include "MZVehiclePawn.h"
#include "MZVehicleDataAsset.h"
#include "MileZero.h"

#include "ChaosWheeledVehicleMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

AMZVehiclePawn::AMZVehiclePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// --- Chase camera rig ---
	ChaseBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("ChaseBoom"));
	ChaseBoom->SetupAttachment(GetMesh());
	ChaseBoom->TargetArmLength = 500.0f;
	ChaseBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	ChaseBoom->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));
	ChaseBoom->bUsePawnControlRotation = false;
	ChaseBoom->bInheritPitch = false;
	ChaseBoom->bInheritRoll = false;
	ChaseBoom->bInheritYaw = true;
	ChaseBoom->bDoCollisionTest = true;
	ChaseBoom->bEnableCameraLag = true;
	ChaseBoom->CameraLagSpeed = 8.0f;
	ChaseBoom->CameraRotationLagSpeed = 6.0f;
	ChaseBoom->bEnableCameraRotationLag = true;

	ChaseCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	ChaseCamera->SetupAttachment(ChaseBoom, USpringArmComponent::SocketName);

	// --- Hood camera ---
	HoodCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("HoodCamera"));
	HoodCamera->SetupAttachment(GetMesh());
	HoodCamera->SetRelativeLocation(FVector(100.0f, 0.0f, 130.0f));
	HoodCamera->SetRelativeRotation(FRotator(-5.0f, 0.0f, 0.0f));
}

void AMZVehiclePawn::BeginPlay()
{
	Super::BeginPlay();

	SpawnTransform = GetActorTransform();
	ActivateCameraMode(EMZCameraMode::Chase);

	// Apply data asset tuning if assigned
	if (VehicleData)
	{
		ApplyVehicleData(VehicleData);
	}

	UE_LOG(LogMileZero, Log, TEXT("MZVehiclePawn spawned at %s"), *SpawnTransform.GetLocation().ToString());
}

void AMZVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// ─── Input binding ──────────────────────────────────────────────

void AMZVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC)
	{
		UE_LOG(LogMileZero, Error, TEXT("MZVehiclePawn requires Enhanced Input Component"));
		return;
	}

	if (IA_Throttle)
	{
		EIC->BindAction(IA_Throttle, ETriggerEvent::Triggered, this, &AMZVehiclePawn::HandleThrottle);
		EIC->BindAction(IA_Throttle, ETriggerEvent::Completed, this, &AMZVehiclePawn::HandleThrottle);
	}
	if (IA_Brake)
	{
		EIC->BindAction(IA_Brake, ETriggerEvent::Triggered, this, &AMZVehiclePawn::HandleBrake);
		EIC->BindAction(IA_Brake, ETriggerEvent::Completed, this, &AMZVehiclePawn::HandleBrake);
	}
	if (IA_Steer)
	{
		EIC->BindAction(IA_Steer, ETriggerEvent::Triggered, this, &AMZVehiclePawn::HandleSteering);
		EIC->BindAction(IA_Steer, ETriggerEvent::Completed, this, &AMZVehiclePawn::HandleSteering);
	}
	if (IA_Handbrake)
	{
		EIC->BindAction(IA_Handbrake, ETriggerEvent::Started, this, &AMZVehiclePawn::HandleHandbrake);
		EIC->BindAction(IA_Handbrake, ETriggerEvent::Completed, this, &AMZVehiclePawn::HandleHandbrakeReleased);
	}
	if (IA_ShiftUp)
	{
		EIC->BindAction(IA_ShiftUp, ETriggerEvent::Started, this, &AMZVehiclePawn::HandleShiftUp);
	}
	if (IA_ShiftDown)
	{
		EIC->BindAction(IA_ShiftDown, ETriggerEvent::Started, this, &AMZVehiclePawn::HandleShiftDown);
	}
	if (IA_CameraCycle)
	{
		EIC->BindAction(IA_CameraCycle, ETriggerEvent::Started, this, &AMZVehiclePawn::HandleCameraCycle);
	}
	if (IA_ResetVehicle)
	{
		EIC->BindAction(IA_ResetVehicle, ETriggerEvent::Started, this, &AMZVehiclePawn::HandleResetVehicle);
	}
	if (IA_Look)
	{
		EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMZVehiclePawn::HandleLook);
	}

	UE_LOG(LogMileZero, Log, TEXT("Vehicle input bindings complete"));
}

// ─── Input handlers ─────────────────────────────────────────────

void AMZVehiclePawn::HandleThrottle(const FInputActionValue& Value)
{
	CachedThrottle = Value.Get<float>();
	GetVehicleMovementComponent()->SetThrottleInput(CachedThrottle);
}

void AMZVehiclePawn::HandleBrake(const FInputActionValue& Value)
{
	CachedBrake = Value.Get<float>();
	GetVehicleMovementComponent()->SetBrakeInput(CachedBrake);
}

void AMZVehiclePawn::HandleSteering(const FInputActionValue& Value)
{
	CachedSteering = Value.Get<float>();
	GetVehicleMovementComponent()->SetSteeringInput(CachedSteering);
}

void AMZVehiclePawn::HandleHandbrake(const FInputActionValue& Value)
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AMZVehiclePawn::HandleHandbrakeReleased(const FInputActionValue& Value)
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void AMZVehiclePawn::HandleShiftUp()
{
	// Chaos vehicles handle automatic transmission; this is for manual override later
	UE_LOG(LogMileZero, Verbose, TEXT("ShiftUp requested"));
}

void AMZVehiclePawn::HandleShiftDown()
{
	UE_LOG(LogMileZero, Verbose, TEXT("ShiftDown requested"));
}

void AMZVehiclePawn::HandleCameraCycle()
{
	CycleCamera();
}

void AMZVehiclePawn::HandleResetVehicle()
{
	ResetVehicle();
}

void AMZVehiclePawn::HandleLook(const FInputActionValue& Value)
{
	if (CurrentCameraMode == EMZCameraMode::Chase && ChaseBoom)
	{
		FVector2D LookInput = Value.Get<FVector2D>();
		FRotator Current = ChaseBoom->GetRelativeRotation();
		Current.Yaw += LookInput.X;
		Current.Pitch = FMath::Clamp(Current.Pitch + LookInput.Y, -60.0f, 10.0f);
		ChaseBoom->SetRelativeRotation(Current);
	}
}

// ─── Camera ─────────────────────────────────────────────────────

void AMZVehiclePawn::CycleCamera()
{
	int32 Next = (static_cast<int32>(CurrentCameraMode) + 1) % static_cast<int32>(EMZCameraMode::Count);
	ActivateCameraMode(static_cast<EMZCameraMode>(Next));
}

void AMZVehiclePawn::ActivateCameraMode(EMZCameraMode Mode)
{
	CurrentCameraMode = Mode;

	if (ChaseCamera) ChaseCamera->SetActive(Mode == EMZCameraMode::Chase);
	if (HoodCamera) HoodCamera->SetActive(Mode == EMZCameraMode::Hood);

	UE_LOG(LogMileZero, Log, TEXT("Camera mode: %d"), static_cast<int32>(Mode));
}

// ─── Telemetry ──────────────────────────────────────────────────

float AMZVehiclePawn::GetSpeedKmh() const
{
	// Chaos returns cm/s; convert to km/h
	return GetVehicleMovementComponent()->GetForwardSpeed() * 0.036f;
}

float AMZVehiclePawn::GetRPM() const
{
	return GetVehicleMovementComponent()->GetEngineRotationSpeed();
}

int32 AMZVehiclePawn::GetCurrentGear() const
{
	return GetVehicleMovementComponent()->GetCurrentGear();
}

// ─── Vehicle data application ───────────────────────────────────

void AMZVehiclePawn::ApplyVehicleData(const UMZVehicleDataAsset* Data)
{
	if (!Data)
	{
		return;
	}

	UChaosWheeledVehicleMovementComponent* Movement = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent());
	if (!Movement)
	{
		UE_LOG(LogMileZero, Warning, TEXT("No ChaosWheeledVehicleMovementComponent found"));
		return;
	}

	// Mass
	if (UPrimitiveComponent* MeshComp = Cast<UPrimitiveComponent>(GetMesh()))
	{
		MeshComp->SetMassOverrideInKg(NAME_None, Data->MassKg, true);
	}

	// Center of mass
	Movement->CenterOfMassOverride = Data->CenterOfMassOffset;
	Movement->bEnableCenterOfMassOverride = true;

	// Engine
	Movement->EngineSetup.MaxTorque = Data->Engine.MaxTorque;
	Movement->EngineSetup.MaxRPM = Data->Engine.MaxRPM;
	Movement->EngineSetup.EngineIdleRPM = Data->Engine.IdleRPM;
	Movement->EngineSetup.EngineRevDownRate = Data->Engine.EngineRevDownRate;

	// Transmission
	Movement->TransmissionSetup.FinalRatio = Data->FinalDriveRatio;
	if (Data->GearRatios.Num() > 0)
	{
		Movement->TransmissionSetup.ForwardGearRatios = Data->GearRatios;
	}

	// Steering
	Movement->SteeringSetup.MaxSteeringAngle = Data->MaxSteerAngle;

	// Drag
	Movement->DragCoefficient = Data->DragCoefficient;

	UE_LOG(LogMileZero, Log, TEXT("Applied vehicle data: %s (%.0f kg, %s)"),
		*Data->DisplayName.ToString(),
		Data->MassKg,
		Data->DrivetrainType == EMZDrivetrainType::RWD ? TEXT("RWD") :
		Data->DrivetrainType == EMZDrivetrainType::FWD ? TEXT("FWD") : TEXT("AWD"));
}

// ─── Reset ──────────────────────────────────────────────────────

void AMZVehiclePawn::ResetVehicle()
{
	UChaosWheeledVehicleMovementComponent* Movement = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent());
	if (Movement)
	{
		// Stop all motion
		Movement->SetThrottleInput(0.0f);
		Movement->SetBrakeInput(1.0f);
		Movement->SetSteeringInput(0.0f);
		Movement->SetHandbrakeInput(true);
	}

	// Teleport back to spawn
	SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::ResetPhysics);

	if (UPrimitiveComponent* MeshComp = Cast<UPrimitiveComponent>(GetMesh()))
	{
		MeshComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
		MeshComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	}

	// Release brakes after teleport
	if (Movement)
	{
		Movement->SetBrakeInput(0.0f);
		Movement->SetHandbrakeInput(false);
	}

	CachedThrottle = 0.0f;
	CachedBrake = 0.0f;
	CachedSteering = 0.0f;

	UE_LOG(LogMileZero, Log, TEXT("Vehicle reset to spawn point"));
}
