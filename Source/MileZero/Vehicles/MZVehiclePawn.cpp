#include "MZVehiclePawn.h"
#include "MZVehicleDataAsset.h"
#include "MZWheelFront.h"
#include "MZWheelRear.h"
#include "Surfaces/MZSurfaceContactComponent.h"
#include "Damage/MZVehicleDamageComponent.h"
#include "Audio/MZRadioManager.h"
#include "Core/MZPlayerController.h"
#include "MileZero.h"

#include "ChaosWheeledVehicleMovementComponent.h"
#include "Components/StaticMeshComponent.h"
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

	// --- Default vehicle mesh (SportsCar from template) ---
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultMesh(
		TEXT("/Game/Vehicles/SportsCar/SKM_SportsCar"));
	if (DefaultMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(DefaultMesh.Object);
		GetMesh()->SetSimulatePhysics(true);
	}

	// --- Animation Blueprint (essential for wheel animation) ---
	static ConstructorHelpers::FClassFinder<UAnimInstance> DefaultAnimBP(
		TEXT("/Game/Vehicles/SportsCar/ABP_SportsCar"));
	if (DefaultAnimBP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(DefaultAnimBP.Class);
	}

	// --- Car body (separate static mesh attached to skeleton root) ---
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetMesh());
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMeshAsset(
		TEXT("/Game/Vehicles/SportsCar/SM_SportsCar"));
	if (BodyMeshAsset.Succeeded())
	{
		BodyMesh->SetStaticMesh(BodyMeshAsset.Object);
	}

	// --- Glass mesh ---
	GlassMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GlassMesh"));
	GlassMesh->SetupAttachment(GetMesh());
	GlassMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GlassMeshAsset(
		TEXT("/Game/Vehicles/SportsCar/SM_SportsCar_Glass"));
	if (GlassMeshAsset.Succeeded())
	{
		GlassMesh->SetStaticMesh(GlassMeshAsset.Object);
	}

	// --- Default wheel setup (SportsCar bone names: Phys_Wheel_XX) ---
	UChaosWheeledVehicleMovementComponent* Movement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent());
	Movement->WheelSetups.SetNum(4);

	Movement->WheelSetups[0].WheelClass = UMZWheelFront::StaticClass();
	Movement->WheelSetups[0].BoneName = FName("Phys_Wheel_FL");

	Movement->WheelSetups[1].WheelClass = UMZWheelFront::StaticClass();
	Movement->WheelSetups[1].BoneName = FName("Phys_Wheel_FR");

	Movement->WheelSetups[2].WheelClass = UMZWheelRear::StaticClass();
	Movement->WheelSetups[2].BoneName = FName("Phys_Wheel_BL");

	Movement->WheelSetups[3].WheelClass = UMZWheelRear::StaticClass();
	Movement->WheelSetups[3].BoneName = FName("Phys_Wheel_BR");

	// --- Wheel meshes (attached to skeleton bone sockets) ---
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WheelMeshAsset(
		TEXT("/Game/Vehicles/SportsCar/SM_SportsCar_Wheel"));

	auto CreateWheelMesh = [&](const TCHAR* Name, FName BoneName, bool bRightSide) -> UStaticMeshComponent*
	{
		UStaticMeshComponent* WM = CreateDefaultSubobject<UStaticMeshComponent>(Name);
		WM->SetupAttachment(GetMesh(), BoneName);
		WM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (WheelMeshAsset.Succeeded())
		{
			WM->SetStaticMesh(WheelMeshAsset.Object);
		}
		if (bRightSide)
		{
			// Mirror right side + flip 180° so tire tread direction is correct
			WM->SetRelativeScale3D(FVector(1.0f, -1.0f, 1.0f));
			WM->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
		}
		return WM;
	};

	WheelMesh_FL = CreateWheelMesh(TEXT("WheelMesh_FL"), FName("Phys_Wheel_FL"), false);
	WheelMesh_FR = CreateWheelMesh(TEXT("WheelMesh_FR"), FName("Phys_Wheel_FR"), true);
	WheelMesh_BL = CreateWheelMesh(TEXT("WheelMesh_BL"), FName("Phys_Wheel_BL"), false);
	WheelMesh_BR = CreateWheelMesh(TEXT("WheelMesh_BR"), FName("Phys_Wheel_BR"), true);

	// --- Chassis physics (realistic sports car) ---
	Movement->ChassisHeight = 140.0f;
	Movement->DragCoefficient = 0.32f;
	Movement->DownforceCoefficient = 0.3f;
	Movement->CenterOfMassOverride = FVector(-5.0f, 0.0f, -40.0f); // low & slightly forward
	Movement->bEnableCenterOfMassOverride = true;
	Movement->bLegacyWheelFrictionPosition = false;

	// Mass — realistic sports car ~1300 kg
	GetMesh()->SetMassOverrideInKg(NAME_None, 1300.0f, true);

	// --- Engine (realistic NA sports car) ---
	Movement->EngineSetup.MaxTorque = 350.0f;
	Movement->EngineSetup.MaxRPM = 7000.0f;
	Movement->EngineSetup.EngineIdleRPM = 800.0f;
	Movement->EngineSetup.EngineRevDownRate = 800.0f;

	// Torque curve — realistic NA profile
	FRichCurve* TorqueCurve = Movement->EngineSetup.TorqueCurve.GetRichCurve();
	TorqueCurve->Reset();
	TorqueCurve->AddKey(0.0f, 0.4f);      // 40% at idle
	TorqueCurve->AddKey(0.15f, 0.65f);    // building torque
	TorqueCurve->AddKey(0.35f, 0.85f);    // mid-range
	TorqueCurve->AddKey(0.55f, 1.0f);     // peak torque
	TorqueCurve->AddKey(0.75f, 0.92f);    // slight drop
	TorqueCurve->AddKey(0.90f, 0.75f);    // falling off
	TorqueCurve->AddKey(1.0f, 0.55f);     // redline

	// --- Transmission (5-speed realistic ratios) ---
	Movement->TransmissionSetup.FinalRatio = 3.7f;
	Movement->TransmissionSetup.ForwardGearRatios.Empty();
	Movement->TransmissionSetup.ForwardGearRatios.Add(3.4f);   // 1st
	Movement->TransmissionSetup.ForwardGearRatios.Add(2.1f);   // 2nd
	Movement->TransmissionSetup.ForwardGearRatios.Add(1.45f);  // 3rd
	Movement->TransmissionSetup.ForwardGearRatios.Add(1.05f);  // 4th
	Movement->TransmissionSetup.ForwardGearRatios.Add(0.80f);  // 5th
	Movement->TransmissionSetup.ReverseGearRatios.Empty();
	Movement->TransmissionSetup.ReverseGearRatios.Add(3.4f);

	// --- Steering ---
	Movement->SteeringSetup.SteeringType = ESteeringType::AngleRatio;
	Movement->SteeringSetup.AngleRatio = 0.8f;

	// --- Transmission: automatic ---
	Movement->TransmissionSetup.bUseAutomaticGears = true;
	Movement->TransmissionSetup.GearChangeTime = 0.25f;
	Movement->TransmissionSetup.TransmissionEfficiency = 0.92f;

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

	// --- Surface contact detection ---
	SurfaceContact = CreateDefaultSubobject<UMZSurfaceContactComponent>(TEXT("SurfaceContact"));

	// --- Damage component ---
	DamageComp = CreateDefaultSubobject<UMZVehicleDamageComponent>(TEXT("DamageComp"));
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

	// Register collision handler for damage system
	OnActorHit.AddDynamic(this, &AMZVehiclePawn::OnVehicleHit);

	// Ensure the bootstrapped mapping context is registered now that the controller is ready.
	// SetupPlayerInputComponent may be called before the input subsystem is fully available,
	// so we re-register here as a safety net.
	if (BootstrappedMappingContext)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				Sub->ClearAllMappings();
				Sub->AddMappingContext(BootstrappedMappingContext, 0);
				UE_LOG(LogMileZero, Log, TEXT("Re-registered driving input context in BeginPlay"));
			}
		}
	}

	UE_LOG(LogMileZero, Log, TEXT("MZVehiclePawn spawned at %s"), *SpawnTransform.GetLocation().ToString());
}

void AMZVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UChaosWheeledVehicleMovementComponent* WheeledMovement = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent());
	if (!WheeledMovement) return;

	// --- Re-apply ALL inputs every tick (Chaos resets per physics step) ---
	WheeledMovement->SetThrottleInput(CachedThrottle);
	WheeledMovement->SetBrakeInput(CachedBrake);
	WheeledMovement->SetSteeringInput(CachedSteering);

	// --- Surface grip ---
	float BaseGrip = SurfaceContact ? SurfaceContact->GetCurrentGripMultiplier() : 1.0f;

	// --- Per-wheel friction: surface * damage ---
	for (int32 i = 0; i < WheeledMovement->GetNumWheels(); ++i)
	{
		float WheelGrip = BaseGrip;
		if (DamageComp)
		{
			WheelGrip *= DamageComp->GetWheelGripMultiplier(i);
		}
		WheeledMovement->SetWheelFrictionMultiplier(i, WheelGrip);
	}

	// --- Damage effects on vehicle dynamics (modifies inputs above) ---
	ApplyDamageEffects(DeltaTime);
}

void AMZVehiclePawn::ApplyDamageEffects(float DeltaTime)
{
	if (!DamageComp) return;

	UChaosWheeledVehicleMovementComponent* Movement = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent());
	if (!Movement) return;

	// Power loss from engine damage + overheat
	float PowerMult = DamageComp->GetPowerMultiplier();
	float OverheatPenalty = DamageComp->GetOverheatFactor() * 0.3f; // up to 30% extra loss from overheat
	float EffectiveTorque = Movement->EngineSetup.MaxTorque * (PowerMult - OverheatPenalty);
	// Note: we don't directly modify MaxTorque per-frame since it's a setup param
	// Instead, reduce throttle input proportionally
	if (CachedThrottle > 0.0f)
	{
		float ThrottleScale = FMath::Max(0.0f, PowerMult - OverheatPenalty);
		Movement->SetThrottleInput(CachedThrottle * ThrottleScale);
	}

	// Drag increase from aero damage
	// Base drag is stored from ApplyVehicleData; we scale it
	float DragMult = DamageComp->GetDragMultiplier();
	// Movement->DragCoefficient is set during ApplyVehicleData, so we don't modify it here
	// Instead the drag effect reduces top speed naturally through the physics system

	// Brake degradation
	if (CachedBrake > 0.0f)
	{
		float BrakeMult = DamageComp->GetBrakeMultiplier();
		Movement->SetBrakeInput(CachedBrake * BrakeMult);
	}

	// Steering pull from damage
	float SteeringPull = DamageComp->GetSteeringPullDegrees();
	if (FMath::Abs(SteeringPull) > 0.5f)
	{
		// Apply as a constant offset to steering — normalised to [-1,1] range
		float PullInput = FMath::Clamp(SteeringPull / 35.0f, -1.0f, 1.0f);
		float AdjustedSteering = FMath::Clamp(CachedSteering + PullInput * 0.3f, -1.0f, 1.0f);
		Movement->SetSteeringInput(AdjustedSteering);
	}
}

void AMZVehiclePawn::OnVehicleHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!DamageComp) return;

	// Calculate impact speed from impulse magnitude and vehicle mass
	float SpeedKmh = FMath::Abs(GetSpeedKmh());

	// Only process significant impacts
	if (SpeedKmh < 5.0f) return;

	DamageComp->ProcessImpact(SpeedKmh, Hit.ImpactNormal, Hit.ImpactPoint);

	UE_LOG(LogMileZero, Log, TEXT("Vehicle impact at %.0f km/h — overall damage: %.0f%%"),
		SpeedKmh, DamageComp->GetOverallDamagePercent());
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
	IA_SteerLeft    = MakeAction(TEXT("IA_MZ_SteerLeft"),    EInputActionValueType::Boolean);
	IA_SteerRight   = MakeAction(TEXT("IA_MZ_SteerRight"),   EInputActionValueType::Boolean);
	IA_Handbrake    = MakeAction(TEXT("IA_MZ_Handbrake"),    EInputActionValueType::Boolean);
	IA_ShiftUp      = MakeAction(TEXT("IA_MZ_ShiftUp"),      EInputActionValueType::Boolean);
	IA_ShiftDown    = MakeAction(TEXT("IA_MZ_ShiftDown"),    EInputActionValueType::Boolean);
	IA_CameraCycle  = MakeAction(TEXT("IA_MZ_CameraCycle"),  EInputActionValueType::Boolean);
	IA_ResetVehicle = MakeAction(TEXT("IA_MZ_ResetVehicle"), EInputActionValueType::Boolean);
	IA_Look         = MakeAction(TEXT("IA_MZ_Look"),         EInputActionValueType::Axis2D);
	IA_ExitVehicle  = MakeAction(TEXT("IA_MZ_ExitVehicle"),  EInputActionValueType::Boolean);
	IA_RadioNext    = MakeAction(TEXT("IA_MZ_RadioNext"),    EInputActionValueType::Boolean);

	// Build mapping context with default keyboard/mouse bindings
	BootstrappedMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_MZ_Vehicle_Default"));

	BootstrappedMappingContext->MapKey(IA_Throttle, EKeys::W);
	BootstrappedMappingContext->MapKey(IA_Brake, EKeys::S);

	// Steer: separate boolean actions — bulletproof
	BootstrappedMappingContext->MapKey(IA_SteerRight, EKeys::D);
	BootstrappedMappingContext->MapKey(IA_SteerLeft, EKeys::A);

	BootstrappedMappingContext->MapKey(IA_Handbrake, EKeys::SpaceBar);
	BootstrappedMappingContext->MapKey(IA_ShiftUp, EKeys::E);
	BootstrappedMappingContext->MapKey(IA_ShiftDown, EKeys::Q);
	BootstrappedMappingContext->MapKey(IA_CameraCycle, EKeys::C);
	BootstrappedMappingContext->MapKey(IA_ResetVehicle, EKeys::R);
	BootstrappedMappingContext->MapKey(IA_ExitVehicle, EKeys::F);
	BootstrappedMappingContext->MapKey(IA_RadioNext, EKeys::Period);
	BootstrappedMappingContext->MapKey(IA_Look, EKeys::Mouse2D);

	// Also add gamepad mappings
	BootstrappedMappingContext->MapKey(IA_Throttle, EKeys::Gamepad_RightTriggerAxis);
	BootstrappedMappingContext->MapKey(IA_Brake, EKeys::Gamepad_LeftTriggerAxis);
	// Gamepad steering handled via IA_Look or custom axis — skip for now
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
	if (IA_SteerLeft)
	{
		EIC->BindAction(IA_SteerLeft, ETriggerEvent::Started, this, &AMZVehiclePawn::HandleSteerLeft);
		EIC->BindAction(IA_SteerLeft, ETriggerEvent::Completed, this, &AMZVehiclePawn::HandleSteerLeftReleased);
	}
	if (IA_SteerRight)
	{
		EIC->BindAction(IA_SteerRight, ETriggerEvent::Started, this, &AMZVehiclePawn::HandleSteerRight);
		EIC->BindAction(IA_SteerRight, ETriggerEvent::Completed, this, &AMZVehiclePawn::HandleSteerRightReleased);
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
	if (IA_ExitVehicle)
	{
		EIC->BindAction(IA_ExitVehicle, ETriggerEvent::Started, this, &AMZVehiclePawn::HandleExitVehicle);
	}
	if (IA_RadioNext)
	{
		EIC->BindAction(IA_RadioNext, ETriggerEvent::Started, this, &AMZVehiclePawn::HandleRadioNext);
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

void AMZVehiclePawn::HandleSteerLeft(const FInputActionValue& Value)
{
	bSteerLeftHeld = true;
	CachedSteering = (bSteerRightHeld ? 1.0f : 0.0f) + (bSteerLeftHeld ? -1.0f : 0.0f);
}

void AMZVehiclePawn::HandleSteerLeftReleased(const FInputActionValue& Value)
{
	bSteerLeftHeld = false;
	CachedSteering = (bSteerRightHeld ? 1.0f : 0.0f) + (bSteerLeftHeld ? -1.0f : 0.0f);
}

void AMZVehiclePawn::HandleSteerRight(const FInputActionValue& Value)
{
	bSteerRightHeld = true;
	CachedSteering = (bSteerRightHeld ? 1.0f : 0.0f) + (bSteerLeftHeld ? -1.0f : 0.0f);
}

void AMZVehiclePawn::HandleSteerRightReleased(const FInputActionValue& Value)
{
	bSteerRightHeld = false;
	CachedSteering = (bSteerRightHeld ? 1.0f : 0.0f) + (bSteerLeftHeld ? -1.0f : 0.0f);
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

void AMZVehiclePawn::HandleExitVehicle()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (class AMZPlayerController* MZController = Cast<class AMZPlayerController>(PC))
		{
			MZController->RequestExitVehicle();
		}
	}
}

void AMZVehiclePawn::HandleRadioNext()
{
	// Get radio manager from game instance
	if (UGameInstance* GI = GetGameInstance())
	{
		if (class UMZRadioManager* RadioManager = GI->GetSubsystem<class UMZRadioManager>())
		{
			RadioManager->NextStation();
		}
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

	// Repair all damage on reset
	if (DamageComp)
	{
		DamageComp->RepairAll();
	}

	UE_LOG(LogMileZero, Log, TEXT("Vehicle reset to spawn point (damage repaired)"));
}

// ─── Surface telemetry ──────────────────────────────────────────

FName AMZVehiclePawn::GetCurrentSurfaceID() const
{
	return SurfaceContact ? SurfaceContact->GetCurrentSurfaceID() : FName("Unknown");
}

float AMZVehiclePawn::GetCurrentGripMultiplier() const
{
	return SurfaceContact ? SurfaceContact->GetCurrentGripMultiplier() : 1.0f;
}

float AMZVehiclePawn::GetDamagePercent() const
{
	return DamageComp ? DamageComp->GetOverallDamagePercent() : 0.0f;
}

FTransform AMZVehiclePawn::GetExitTransform() const
{
	// Calculate world position from vehicle-relative offset
	FVector WorldExitPos = GetActorLocation() + GetActorRotation().RotateVector(ExitOffset);
	return FTransform(GetActorRotation(), WorldExitPos);
}
