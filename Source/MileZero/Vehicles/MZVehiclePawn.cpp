#include "MZVehiclePawn.h"
#include "MZVehicleDataAsset.h"
#include "MZWheelFront.h"
#include "MZWheelRear.h"
#include "MileZero.h"

#include "ChaosWheeledVehicleMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"

AMZVehiclePawn::AMZVehiclePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// --- Default vehicle mesh (OffroadCar from template) ---
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultMesh(
		TEXT("/Game/Vehicles/OffroadCar/SKM_Offroad"));
	if (DefaultMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(DefaultMesh.Object);
		GetMesh()->SetSimulatePhysics(true);
	}

	// --- Default wheel setup ---
	UChaosWheeledVehicleMovementComponent* Movement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent());
	Movement->WheelSetups.SetNum(4);

	Movement->WheelSetups[0].WheelClass = UMZWheelFront::StaticClass();
	Movement->WheelSetups[0].BoneName = FName("PhysWheel_FL");

	Movement->WheelSetups[1].WheelClass = UMZWheelFront::StaticClass();
	Movement->WheelSetups[1].BoneName = FName("PhysWheel_FR");

	Movement->WheelSetups[2].WheelClass = UMZWheelRear::StaticClass();
	Movement->WheelSetups[2].BoneName = FName("PhysWheel_BL");

	Movement->WheelSetups[3].WheelClass = UMZWheelRear::StaticClass();
	Movement->WheelSetups[3].BoneName = FName("PhysWheel_BR");

	// --- Default chassis settings ---
	Movement->ChassisHeight = 140.0f;
	Movement->DragCoefficient = 0.35f;
	Movement->DownforceCoefficient = 0.1f;
	Movement->CenterOfMassOverride = FVector(0.0f, 0.0f, -15.0f);
	Movement->bEnableCenterOfMassOverride = true;
	Movement->bLegacyWheelFrictionPosition = false;

	// --- Default engine/transmission ---
	Movement->EngineSetup.MaxTorque = 400.0f;
	Movement->EngineSetup.MaxRPM = 6800.0f;
	Movement->EngineSetup.EngineIdleRPM = 900.0f;
	Movement->EngineSetup.EngineRevDownRate = 600.0f;

	Movement->TransmissionSetup.FinalRatio = 3.9f;
	Movement->TransmissionSetup.ForwardGearRatios.Empty();
	Movement->TransmissionSetup.ForwardGearRatios.Add(3.6f);
	Movement->TransmissionSetup.ForwardGearRatios.Add(2.1f);
	Movement->TransmissionSetup.ForwardGearRatios.Add(1.4f);
	Movement->TransmissionSetup.ForwardGearRatios.Add(1.0f);
	Movement->TransmissionSetup.ForwardGearRatios.Add(0.77f);

	// --- Default steering ---
	Movement->SteeringSetup.SteeringType = ESteeringType::AngleRatio;
	Movement->SteeringSetup.AngleRatio = 0.7f;

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

// ─── Default input bootstrap ────────────────────────────────────

void AMZVehiclePawn::BootstrapDefaultInput()
{
	UE_LOG(LogMileZero, Log, TEXT("Bootstrapping default vehicle input (no editor assets assigned)"));

	auto MakeAction = [this](FName Name, EInputActionValueType ValueType) -> UInputAction*
	{
		UInputAction* Action = NewObject<UInputAction>(this, Name);
		Action->ValueType = ValueType;
		return Action;
	};

	IA_Throttle     = MakeAction(TEXT("IA_MZ_Throttle"),     EInputActionValueType::Axis1D);
	IA_Brake        = MakeAction(TEXT("IA_MZ_Brake"),        EInputActionValueType::Axis1D);
	IA_Steer        = MakeAction(TEXT("IA_MZ_Steer"),        EInputActionValueType::Axis1D);
	IA_Handbrake    = MakeAction(TEXT("IA_MZ_Handbrake"),    EInputActionValueType::Boolean);
	IA_ShiftUp      = MakeAction(TEXT("IA_MZ_ShiftUp"),      EInputActionValueType::Boolean);
	IA_ShiftDown    = MakeAction(TEXT("IA_MZ_ShiftDown"),    EInputActionValueType::Boolean);
	IA_CameraCycle  = MakeAction(TEXT("IA_MZ_CameraCycle"),  EInputActionValueType::Boolean);
	IA_ResetVehicle = MakeAction(TEXT("IA_MZ_ResetVehicle"), EInputActionValueType::Boolean);
	IA_Look         = MakeAction(TEXT("IA_MZ_Look"),         EInputActionValueType::Axis2D);

	// Build mapping context with default keyboard/mouse bindings
	BootstrappedMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_MZ_Vehicle_Default"));

	BootstrappedMappingContext->MapKey(IA_Throttle, EKeys::W);
	BootstrappedMappingContext->MapKey(IA_Brake, EKeys::S);

	// Steer: D = positive, A = negative (negate modifier)
	BootstrappedMappingContext->MapKey(IA_Steer, EKeys::D);
	FEnhancedActionKeyMapping& SteerNeg = BootstrappedMappingContext->MapKey(IA_Steer, EKeys::A);
	SteerNeg.Modifiers.Add(NewObject<UInputModifierNegate>(this));

	BootstrappedMappingContext->MapKey(IA_Handbrake, EKeys::SpaceBar);
	BootstrappedMappingContext->MapKey(IA_ShiftUp, EKeys::E);
	BootstrappedMappingContext->MapKey(IA_ShiftDown, EKeys::Q);
	BootstrappedMappingContext->MapKey(IA_CameraCycle, EKeys::C);
	BootstrappedMappingContext->MapKey(IA_ResetVehicle, EKeys::R);
	BootstrappedMappingContext->MapKey(IA_Look, EKeys::Mouse2D);

	// Also add gamepad mappings
	BootstrappedMappingContext->MapKey(IA_Throttle, EKeys::Gamepad_RightTriggerAxis);
	BootstrappedMappingContext->MapKey(IA_Brake, EKeys::Gamepad_LeftTriggerAxis);
	BootstrappedMappingContext->MapKey(IA_Steer, EKeys::Gamepad_LeftX);
	BootstrappedMappingContext->MapKey(IA_Handbrake, EKeys::Gamepad_FaceButton_Bottom);
	BootstrappedMappingContext->MapKey(IA_ResetVehicle, EKeys::Gamepad_FaceButton_Top);
	BootstrappedMappingContext->MapKey(IA_CameraCycle, EKeys::Gamepad_FaceButton_Right);
	BootstrappedMappingContext->MapKey(IA_Look, EKeys::Gamepad_Right2D);

	// Register the mapping context
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Sub->AddMappingContext(BootstrappedMappingContext, 0);
			UE_LOG(LogMileZero, Log, TEXT("Default driving input context registered"));
		}
	}
}

// ─── Input binding ──────────────────────────────────────────────

void AMZVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bootstrap input actions and mapping context if none assigned in editor
	if (!IA_Throttle)
	{
		BootstrapDefaultInput();
	}

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
	if (const UChaosWheeledVehicleMovementComponent* WheeledMC = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent()))
	{
		return WheeledMC->GetEngineRotationSpeed();
	}
	return 0.0f;
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

	// Steering — set per-wheel max steer angle (front wheels typically 0 and 1)
	for (int32 i = 0; i < Movement->GetNumWheels(); ++i)
	{
		if (i < 2) // front axle
		{
			Movement->SetWheelMaxSteerAngle(i, Data->MaxSteerAngle);
		}
	}

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
