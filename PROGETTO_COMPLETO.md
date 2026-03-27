# MILEZERO — DOCUMENTAZIONE COMPLETA DEL PROGETTO

> Generato il 2026-03-27. Questo file contiene OGNI dettaglio del progetto MileZero:
> struttura, codice sorgente, configurazioni, documentazione, asset, tool, stato corrente,
> ambiente di sviluppo, cronologia git, e decisioni architetturali.

---

## INDICE

1. [Identita' del progetto](#1-identita-del-progetto)
2. [Ambiente di sviluppo](#2-ambiente-di-sviluppo)
3. [Cronologia Git](#3-cronologia-git)
4. [Struttura completa dei file](#4-struttura-completa-dei-file)
5. [File di progetto UE5](#5-file-di-progetto-ue5)
6. [Codice sorgente completo](#6-codice-sorgente-completo)
7. [File di configurazione](#7-file-di-configurazione)
8. [Script e tool](#8-script-e-tool)
9. [Asset binari (Content)](#9-asset-binari-content)
10. [Documentazione di progetto](#10-documentazione-di-progetto)
11. [Stato corrente e milestone](#11-stato-corrente-e-milestone)
12. [Come continuare lo sviluppo](#12-come-continuare-lo-sviluppo)

---

## 1. IDENTITA' DEL PROGETTO

- **Nome**: MileZero
- **Tipo**: Driving sandbox/simulator single-player per PC
- **Engine**: Unreal Engine 5.7
- **Piattaforma**: Windows
- **Approccio**: C++ first, Blueprint solo dove necessario
- **Pilastro**: guida physics-first con conseguenze reali dei crash
- **Obiettivo v1**: vertical slice giocabile con 3 veicoli, 4 superfici, 4 attivita', danni meccanici, traffico AI, ciclo giorno/notte, save/load

### Veicoli target
| Veicolo | Massa | Trazione | Carattere |
|---------|-------|----------|-----------|
| Coupe | 1250-1350 kg | RWD | Agile, rotation-friendly |
| Hatchback | 1100-1250 kg | FWD | Stabile, forgiving |
| Pickup/SUV | 1800-2200 kg | AWD | Piu' rollio, transizioni lente |

### Superfici target
| Superficie | Grip | Note |
|-----------|------|------|
| Dry Asphalt | 1.00 | Riferimento |
| Wet Asphalt | 0.78 | Slip aumentato |
| Gravel | 0.62 | Frenata lunga |
| Grass | 0.42 | Bassa aderenza |

---

## 2. AMBIENTE DI SVILUPPO

### Hardware/OS
- **OS**: Windows 11 Pro 10.0.26100
- **Shell**: bash (Git Bash / MSYS2)

### Toolchain installata (verificata 2026-03-27)
- **Unreal Engine 5.7.4** → `C:\Program Files\Epic Games\UE_5.7`
- **Visual Studio 2026 Community (18 Insiders)** → `C:\Program Files\Microsoft Visual Studio\18\Insiders`
  - MSVC toolset: 14.44.35207 (usato per compilazione), 14.50, 14.51
  - Workload: Desktop development with C++
  - MSBuild: presente
- **Windows SDK**: 10.0.26100.0
- **.NET**: bundled con UE5 (8.0.412)
- **VS2019 BuildTools** (legacy, non usato): `C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools`

### Build settings UE5
- `BuildSettingsVersion.V6`
- `EngineIncludeOrderVersion.Unreal5_7`
- PCH: `UseExplicitOrSharedPCHs`
- Compilatore: MSVC 14.44 via VS 2026

### Sviluppato con
- **Claude Code** (Anthropic CLI) — modello Claude Opus 4.6 (1M context)
- Tutto il codice C++, config, doc, e tool generati via Claude Code
- Asset binari copiati dai template UE 5.7

---

## 3. CRONOLOGIA GIT

```
51143b1 fix: add LightmassImportanceVolume to test level
bf47baf fix: add SkyAtmosphere + HeightFog to test level, fix sky black warning
d2312ca docs: vehicle is fully self-configuring, only PIE test remains for M1
2130ad2 M1: Self-configuring vehicle with default mesh, wheels, and input
e37164f M0 VERIFIED: first clean compile on UE 5.7 + VS 2026, M1 in progress
3774c01 Initial commit: MileZero Master Prompt Pack
```

---

## 4. STRUTTURA COMPLETA DEI FILE

```
MileZero/
├── CLAUDE.md                          — Contratto del progetto (regole, scope, milestone)
├── MileZero.uproject                  — File progetto UE5.7
├── .gitignore                         — Esclude binari, intermediate, .uasset/.umap
│
├── Source/
│   ├── MileZero.Target.cs             — Target gioco
│   ├── MileZeroEditor.Target.cs       — Target editor
│   └── MileZero/
│       ├── MileZero.Build.cs          — Dipendenze modulo
│       ├── MileZero.h                 — Log category
│       ├── MileZero.cpp               — Implementazione modulo
│       ├── Core/
│       │   ├── MZGameMode.h/.cpp      — Game mode, spawn veicolo
│       │   ├── MZPlayerController.h/.cpp — Input context, enter/exit vehicle
│       │   └── MZGameInstance.h/.cpp   — Stato persistente
│       ├── Characters/
│       │   └── MZCharacter.h/.cpp      — Personaggio a piedi (terza persona)
│       ├── Vehicles/
│       │   ├── MZVehiclePawn.h/.cpp    — Veicolo Chaos (input, camera, telemetria)
│       │   ├── MZVehicleDataAsset.h    — Data asset tuning veicolo
│       │   ├── MZWheelFront.h/.cpp     — Classe ruota anteriore
│       │   └── MZWheelRear.h/.cpp      — Classe ruota posteriore
│       ├── Surfaces/
│       │   ├── MZSurfaceDataAsset.h    — Data asset profilo superficie
│       │   └── MZSurfaceContactComponent.h/.cpp — Rilevamento superficie
│       ├── Activities/
│       │   └── MZActivityDataAsset.h   — Data asset definizione attivita'
│       ├── Debug/
│       │   └── MZDebugSubsystem.h/.cpp — Comandi console MZ.*
│       ├── Save/
│       │   └── MZSaveGame.h/.cpp       — Salvataggio gioco
│       └── Setup/
│           └── MZSetupCommandlet.h/.cpp — Generazione automatica livello test
│
├── Config/
│   ├── DefaultEngine.ini              — Mappa default, game mode, fisica
│   ├── DefaultGame.ini                — Asset Manager, scanning types
│   ├── DefaultInput.ini               — Enhanced Input abilitato
│   └── DefaultEditorPerProjectUserSettings.ini
│
├── Content/
│   ├── MileZero/Maps/Test/
│   │   ├── L_MZ_TestTrack.umap        — Livello test (generato da commandlet)
│   │   └── L_MZ_TestTrack_BuiltData.uasset
│   └── Vehicles/                       — Asset template copiati da UE 5.7
│       ├── OffroadCar/                 — SKM_Offroad + materiali + texture
│       ├── SportsCar/                  — SKM_SportsCar + materiali + texture
│       └── PhysicsMaterials/           — NonSlippery, Slippery
│
├── tools/
│   ├── compile_check.bat              — Compilazione rapida via UBT
│   ├── generate_project_files.bat     — Genera file progetto VS
│   ├── open_editor.bat                — Apre l'editor UE5
│   ├── build.bat                      — Build completo (cook+stage)
│   └── setup_editor_assets.py         — Script Python per creazione asset
│
└── docs/                               — 20 file di documentazione
    ├── STATE.md                        — Stato corrente
    ├── TODO.md                         — Task immediati e futuri
    ├── ROADMAP_EXECUTION.md            — Milestone M0-M8 dettagliati
    ├── MILESTONE_STATUS.md             — Tabella stato milestone
    ├── VERIFICATION.md                 — Log verifiche
    ├── DECISIONS.md                    — Decisioni architetturali
    ├── ARCHITECTURE.md                 — Struttura moduli e classi
    ├── EDITOR_BOOTSTRAP.md             — Guida setup editor
    ├── BUILD_RUNBOOK.md                — Guida compilazione
    ├── KNOWN_ISSUES.md                 — Problemi noti e risoluzioni
    ├── ASSET_MANIFEST.md               — Inventario asset pianificati
    ├── TEST_MATRIX.md                  — Matrice test M1-M7
    ├── RISK_REGISTER.md                — Registro rischi
    ├── PHYSICS_TUNING.md               — Note tuning fisica
    ├── PROJECT_VISION.md               — Visione del prodotto
    └── ...altri doc di supporto
```

---

## 5. FILE DI PROGETTO UE5

### MileZero.uproject
```json
{
    "FileVersion": 3,
    "EngineAssociation": "5.7",
    "Category": "",
    "Description": "MileZero — physics-first driving sandbox",
    "Modules": [
        {
            "Name": "MileZero",
            "Type": "Runtime",
            "LoadingPhase": "Default"
        }
    ],
    "Plugins": [
        { "Name": "ChaosVehiclesPlugin", "Enabled": true },
        { "Name": "EnhancedInput", "Enabled": true },
        { "Name": "DataValidation", "Enabled": true }
    ]
}
```

### .gitignore
```
Binaries/
DerivedDataCache/
Intermediate/
Saved/
Build/
.vs/
.idea/
*.sln
*.suo
*.vcxproj
*.vcxproj.filters
*.vcxproj.user
Thumbs.db
.DS_Store
Desktop.ini
*.uasset
*.umap
CrashReportClient/
.claude/settings.local.json
```

---

## 6. CODICE SORGENTE COMPLETO

### 6.1 Build System

#### Source/MileZero.Target.cs
```csharp
using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.All)]
public class MileZeroTarget : TargetRules
{
    public MileZeroTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V6;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
        ExtraModuleNames.AddRange(new string[] { "MileZero" });
    }
}
```

#### Source/MileZeroEditor.Target.cs
```csharp
using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.All)]
public class MileZeroEditorTarget : TargetRules
{
    public MileZeroEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V6;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
        ExtraModuleNames.AddRange(new string[] { "MileZero" });
    }
}
```

#### Source/MileZero/MileZero.Build.cs
```csharp
using UnrealBuildTool;

public class MileZero : ModuleRules
{
    public MileZero(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Module root must be explicitly added for V6 build settings
        // so subdirectory files can find headers in the module root
        PrivateIncludePaths.Add(ModuleDirectory);

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "ChaosVehicles",
            "PhysicsCore",
            "UMG",
            "Slate",
            "SlateCore"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "ChaosVehiclesCore"
        });

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "UnrealEd"
            });
        }
    }
}
```

### 6.2 Modulo principale

#### Source/MileZero/MileZero.h
```cpp
#pragma once
#include "CoreMinimal.h"
DECLARE_LOG_CATEGORY_EXTERN(LogMileZero, Log, All);
```

#### Source/MileZero/MileZero.cpp
```cpp
#include "MileZero.h"
#include "Modules/ModuleManager.h"
IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, MileZero, "MileZero");
DEFINE_LOG_CATEGORY(LogMileZero);
```

### 6.3 Core

#### Source/MileZero/Core/MZGameMode.h
```cpp
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MZGameMode.generated.h"

UCLASS()
class MILEZERO_API AMZGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AMZGameMode();
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
protected:
    virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
};
```

#### Source/MileZero/Core/MZGameMode.cpp
```cpp
#include "MZGameMode.h"
#include "MZPlayerController.h"
#include "MileZero/Characters/MZCharacter.h"
#include "MileZero/Vehicles/MZVehiclePawn.h"
#include "MileZero.h"
#include "UObject/ConstructorHelpers.h"

AMZGameMode::AMZGameMode()
{
    PlayerControllerClass = AMZPlayerController::StaticClass();
    DefaultPawnClass = AMZVehiclePawn::StaticClass();
}

void AMZGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
    UE_LOG(LogMileZero, Log, TEXT("MZGameMode::InitGame — Map: %s"), *MapName);
}

APawn* AMZGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
    return Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
}
```

#### Source/MileZero/Core/MZPlayerController.h
```cpp
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MZPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class MILEZERO_API AMZPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    AMZPlayerController();
    void SwitchToOnFootInput();
    void SwitchToDrivingInput();
    void RequestEnterVehicle();
    void RequestExitVehicle();
    void ToggleTelemetry();
protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
    TObjectPtr<UInputMappingContext> OnFootMappingContext;
    UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
    TObjectPtr<UInputMappingContext> DrivingMappingContext;
    UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
    TObjectPtr<UInputMappingContext> UIMappingContext;
private:
    void RemoveAllMZContexts();
    bool bIsDriving = false;
};
```

#### Source/MileZero/Core/MZPlayerController.cpp
```cpp
#include "MZPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MileZero.h"

AMZPlayerController::AMZPlayerController() {}

void AMZPlayerController::BeginPlay()
{
    Super::BeginPlay();
    SwitchToDrivingInput();
}

void AMZPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
}

void AMZPlayerController::SwitchToOnFootInput()
{
    RemoveAllMZContexts();
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (OnFootMappingContext) Subsystem->AddMappingContext(OnFootMappingContext, 0);
        if (UIMappingContext) Subsystem->AddMappingContext(UIMappingContext, 1);
    }
    bIsDriving = false;
    UE_LOG(LogMileZero, Log, TEXT("Switched to on-foot input"));
}

void AMZPlayerController::SwitchToDrivingInput()
{
    RemoveAllMZContexts();
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (DrivingMappingContext) Subsystem->AddMappingContext(DrivingMappingContext, 0);
        if (UIMappingContext) Subsystem->AddMappingContext(UIMappingContext, 1);
    }
    bIsDriving = true;
    UE_LOG(LogMileZero, Log, TEXT("Switched to driving input"));
}

void AMZPlayerController::RequestEnterVehicle()
{
    UE_LOG(LogMileZero, Log, TEXT("RequestEnterVehicle — stubbed until on-foot character exists"));
}

void AMZPlayerController::RequestExitVehicle()
{
    UE_LOG(LogMileZero, Log, TEXT("RequestExitVehicle — stubbed until on-foot character exists"));
}

void AMZPlayerController::ToggleTelemetry()
{
    UE_LOG(LogMileZero, Log, TEXT("ToggleTelemetry — wired in M2"));
}

void AMZPlayerController::RemoveAllMZContexts()
{
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (OnFootMappingContext) Subsystem->RemoveMappingContext(OnFootMappingContext);
        if (DrivingMappingContext) Subsystem->RemoveMappingContext(DrivingMappingContext);
    }
}
```

#### Source/MileZero/Core/MZGameInstance.h
```cpp
#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MZGameInstance.generated.h"

UCLASS()
class MILEZERO_API UMZGameInstance : public UGameInstance
{
    GENERATED_BODY()
public:
    virtual void Init() override;
    UPROPERTY(BlueprintReadWrite, Category = "MZ|Game")
    FName SelectedVehicleID = FName("Hatch");
};
```

#### Source/MileZero/Core/MZGameInstance.cpp
```cpp
#include "MZGameInstance.h"
#include "MileZero.h"

void UMZGameInstance::Init()
{
    Super::Init();
    UE_LOG(LogMileZero, Log, TEXT("MZGameInstance initialized — default vehicle: %s"), *SelectedVehicleID.ToString());
}
```

### 6.4 Characters

#### Source/MileZero/Characters/MZCharacter.h
```cpp
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MZCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

UCLASS()
class MILEZERO_API AMZCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    AMZCharacter();
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
protected:
    virtual void BeginPlay() override;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Camera")
    TObjectPtr<UCameraComponent> FollowCamera;
    UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
    TObjectPtr<UInputAction> IA_Move;
    UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
    TObjectPtr<UInputAction> IA_Look;
    UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
    TObjectPtr<UInputAction> IA_Jump;
    UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
    TObjectPtr<UInputAction> IA_Interact;
private:
    void HandleMove(const FInputActionValue& Value);
    void HandleLook(const FInputActionValue& Value);
    void HandleJump();
    void HandleInteract();
};
```

#### Source/MileZero/Characters/MZCharacter.cpp
```cpp
#include "MZCharacter.h"
#include "MileZero.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

AMZCharacter::AMZCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f;
    CameraBoom->bUsePawnControlRotation = true;
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->MaxWalkSpeed = 400.0f;
    GetCharacterMovement()->JumpZVelocity = 420.0f;
}

void AMZCharacter::BeginPlay() { Super::BeginPlay(); }

void AMZCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EIC) return;
    if (IA_Move) EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMZCharacter::HandleMove);
    if (IA_Look) EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMZCharacter::HandleLook);
    if (IA_Jump) EIC->BindAction(IA_Jump, ETriggerEvent::Started, this, &AMZCharacter::HandleJump);
    if (IA_Interact) EIC->BindAction(IA_Interact, ETriggerEvent::Started, this, &AMZCharacter::HandleInteract);
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

void AMZCharacter::HandleJump() { Jump(); }

void AMZCharacter::HandleInteract()
{
    UE_LOG(LogMileZero, Log, TEXT("Interact — vehicle enter/exit wired post-M1"));
}
```

### 6.5 Vehicles (FILE PRINCIPALE — 350+ righe)

#### Source/MileZero/Vehicles/MZVehiclePawn.h
```cpp
#pragma once
#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "MZVehiclePawn.generated.h"

class UChaosWheeledVehicleMovementComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UMZVehicleDataAsset;
struct FInputActionValue;

UENUM(BlueprintType)
enum class EMZCameraMode : uint8
{
    Chase,
    Hood,
    Count UMETA(Hidden)
};

UCLASS()
class MILEZERO_API AMZVehiclePawn : public AWheeledVehiclePawn
{
    GENERATED_BODY()
public:
    AMZVehiclePawn();
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "MZ|Vehicle")
    void ApplyVehicleData(const UMZVehicleDataAsset* Data);
    UFUNCTION(BlueprintCallable, Category = "MZ|Vehicle")
    void ResetVehicle();
    UFUNCTION(BlueprintCallable, Category = "MZ|Vehicle")
    void CycleCamera();

    UFUNCTION(BlueprintPure, Category = "MZ|Telemetry")
    float GetSpeedKmh() const;
    UFUNCTION(BlueprintPure, Category = "MZ|Telemetry")
    float GetRPM() const;
    UFUNCTION(BlueprintPure, Category = "MZ|Telemetry")
    int32 GetCurrentGear() const;
    UFUNCTION(BlueprintPure, Category = "MZ|Telemetry")
    float GetThrottleInput() const { return CachedThrottle; }
    UFUNCTION(BlueprintPure, Category = "MZ|Telemetry")
    float GetBrakeInput() const { return CachedBrake; }
    UFUNCTION(BlueprintPure, Category = "MZ|Telemetry")
    float GetSteeringInput() const { return CachedSteering; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Camera")
    TObjectPtr<USpringArmComponent> ChaseBoom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Camera")
    TObjectPtr<UCameraComponent> ChaseCamera;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MZ|Camera")
    TObjectPtr<UCameraComponent> HoodCamera;

    UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
    TObjectPtr<UInputAction> IA_Throttle;
    UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
    TObjectPtr<UInputAction> IA_Brake;
    UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
    TObjectPtr<UInputAction> IA_Steer;
    UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
    TObjectPtr<UInputAction> IA_Handbrake;
    UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
    TObjectPtr<UInputAction> IA_ShiftUp;
    UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
    TObjectPtr<UInputAction> IA_ShiftDown;
    UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
    TObjectPtr<UInputAction> IA_CameraCycle;
    UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
    TObjectPtr<UInputAction> IA_ResetVehicle;
    UPROPERTY(EditDefaultsOnly, Category = "MZ|Input")
    TObjectPtr<UInputAction> IA_Look;

    UPROPERTY()
    TObjectPtr<UInputMappingContext> BootstrappedMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MZ|Vehicle")
    TObjectPtr<UMZVehicleDataAsset> VehicleData;

private:
    void HandleThrottle(const FInputActionValue& Value);
    void HandleBrake(const FInputActionValue& Value);
    void HandleSteering(const FInputActionValue& Value);
    void HandleHandbrake(const FInputActionValue& Value);
    void HandleHandbrakeReleased(const FInputActionValue& Value);
    void HandleShiftUp();
    void HandleShiftDown();
    void HandleCameraCycle();
    void HandleResetVehicle();
    void HandleLook(const FInputActionValue& Value);
    void BootstrapDefaultInput();
    void ActivateCameraMode(EMZCameraMode Mode);

    UPROPERTY()
    EMZCameraMode CurrentCameraMode = EMZCameraMode::Chase;
    float CachedThrottle = 0.0f;
    float CachedBrake = 0.0f;
    float CachedSteering = 0.0f;
    FTransform SpawnTransform;
};
```

#### Source/MileZero/Vehicles/MZVehiclePawn.cpp
(Vedere il file sorgente per il codice completo — ~350 righe. Punti chiave:)
- **Costruttore**: carica SKM_Offroad, configura 4 ruote (PhysWheel_FL/FR/BL/BR), imposta motore/trasmissione/sterzo, crea camera chase+hood
- **BootstrapDefaultInput()**: crea 9 UInputAction + UInputMappingContext a runtime con bind W/S/A/D/Space/E/Q/C/R/Mouse + gamepad
- **SetupPlayerInputComponent()**: chiama bootstrap se IA_Throttle e' null, poi fa bind di tutti gli input
- **ApplyVehicleData()**: applica tuning dal data asset (massa, motore, trasmissione, sterzo, drag)
- **ResetVehicle()**: teleporta al punto di spawn, azzera velocita'
- **Telemetria**: GetSpeedKmh (cm/s -> km/h), GetRPM (cast a UChaosWheeledVehicleMovementComponent), GetCurrentGear

#### Source/MileZero/Vehicles/MZVehicleDataAsset.h
```cpp
#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MZVehicleDataAsset.generated.h"

UENUM(BlueprintType)
enum class EMZDrivetrainType : uint8
{
    RWD UMETA(DisplayName = "Rear-Wheel Drive"),
    FWD UMETA(DisplayName = "Front-Wheel Drive"),
    AWD UMETA(DisplayName = "All-Wheel Drive")
};

USTRUCT(BlueprintType)
struct FMZWheelSetup
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere) float SpringRate = 25000.0f;
    UPROPERTY(EditAnywhere) float DampingRateBump = 3000.0f;
    UPROPERTY(EditAnywhere) float DampingRateRebound = 4500.0f;
    UPROPERTY(EditAnywhere) float SuspensionMaxDrop = 10.0f;
    UPROPERTY(EditAnywhere) float SuspensionMaxRaise = 10.0f;
    UPROPERTY(EditAnywhere) float WheelRadius = 35.0f;
    UPROPERTY(EditAnywhere) float WheelWidth = 22.0f;
    UPROPERTY(EditAnywhere) float LateralStiffness = 1000.0f;
    UPROPERTY(EditAnywhere) float LongitudinalStiffness = 1000.0f;
};

USTRUCT(BlueprintType)
struct FMZEngineSetup
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere) float MaxTorque = 400.0f;
    UPROPERTY(EditAnywhere) float MaxRPM = 7000.0f;
    UPROPERTY(EditAnywhere) float IdleRPM = 900.0f;
    UPROPERTY(EditAnywhere) float EngineRevDownRate = 2000.0f;
};

USTRUCT(BlueprintType)
struct FMZBrakeSetup
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere) float MaxBrakeTorque = 3000.0f;
    UPROPERTY(EditAnywhere) float HandbrakeTorque = 5000.0f;
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float FrontBrakeBias = 0.6f;
};

UCLASS(BlueprintType)
class MILEZERO_API UMZVehicleDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere) FName VehicleID;
    UPROPERTY(EditAnywhere) FText DisplayName;
    UPROPERTY(EditAnywhere) float MassKg = 1200.0f;
    UPROPERTY(EditAnywhere) FVector CenterOfMassOffset = FVector(0, 0, -15);
    UPROPERTY(EditAnywhere) float DragCoefficient = 0.35f;
    UPROPERTY(EditAnywhere) EMZDrivetrainType DrivetrainType = EMZDrivetrainType::FWD;
    UPROPERTY(EditAnywhere) TArray<float> GearRatios;
    UPROPERTY(EditAnywhere) float FinalDriveRatio = 3.5f;
    UPROPERTY(EditAnywhere) float MaxSteerAngle = 35.0f;
    UPROPERTY(EditAnywhere) float SteeringSpeed = 5.0f;
    UPROPERTY(EditAnywhere) FMZEngineSetup Engine;
    UPROPERTY(EditAnywhere) FMZBrakeSetup Brakes;
    UPROPERTY(EditAnywhere) FMZWheelSetup FrontWheels;
    UPROPERTY(EditAnywhere) FMZWheelSetup RearWheels;
    UPROPERTY(EditAnywhere) TSoftObjectPtr<USkeletalMesh> VehicleMesh;

    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    { return FPrimaryAssetId("MZVehicle", VehicleID); }
};
```

#### Source/MileZero/Vehicles/MZWheelFront.h + .cpp
```cpp
// MZWheelFront.h
#pragma once
#include "CoreMinimal.h"
#include "ChaosVehicleWheel.h"
#include "MZWheelFront.generated.h"

UCLASS()
class MILEZERO_API UMZWheelFront : public UChaosVehicleWheel
{
    GENERATED_BODY()
public:
    UMZWheelFront();
};

// MZWheelFront.cpp
#include "MZWheelFront.h"
UMZWheelFront::UMZWheelFront()
{
    WheelRadius = 40.0f;
    CorneringStiffness = 750.0f;
    FrictionForceMultiplier = 4.0f;
    bAffectedByEngine = true;
    SuspensionMaxRaise = 15.0f;
    SuspensionMaxDrop = 15.0f;
    WheelLoadRatio = 1.0f;
    SpringRate = 120.0f;
    SpringPreload = 100.0f;
    SweepShape = ESweepShape::Shapecast;
    MaxBrakeTorque = 3000.0f;
    MaxHandBrakeTorque = 0.0f;
}
```

#### Source/MileZero/Vehicles/MZWheelRear.h + .cpp
```cpp
// MZWheelRear.h
#pragma once
#include "CoreMinimal.h"
#include "ChaosVehicleWheel.h"
#include "MZWheelRear.generated.h"

UCLASS()
class MILEZERO_API UMZWheelRear : public UChaosVehicleWheel
{
    GENERATED_BODY()
public:
    UMZWheelRear();
};

// MZWheelRear.cpp
#include "MZWheelRear.h"
UMZWheelRear::UMZWheelRear()
{
    WheelRadius = 40.0f;
    CorneringStiffness = 750.0f;
    FrictionForceMultiplier = 4.0f;
    bAffectedByEngine = false;
    SuspensionMaxRaise = 15.0f;
    SuspensionMaxDrop = 15.0f;
    WheelLoadRatio = 1.0f;
    SpringRate = 120.0f;
    SpringPreload = 100.0f;
    SweepShape = ESweepShape::Shapecast;
    MaxBrakeTorque = 2000.0f;
    MaxHandBrakeTorque = 6000.0f;
}
```

### 6.6 Surfaces

#### Source/MileZero/Surfaces/MZSurfaceDataAsset.h
```cpp
#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "MZSurfaceDataAsset.generated.h"

UCLASS(BlueprintType)
class MILEZERO_API UMZSurfaceDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere) FName SurfaceID;
    UPROPERTY(EditAnywhere) FText DisplayName;
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "2.0"))
    float GripMultiplier = 1.0f;
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "5.0"))
    float RollingResistance = 1.0f;
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Roughness = 0.0f;
    UPROPERTY(EditAnywhere)
    TSoftObjectPtr<UPhysicalMaterial> PhysicalMaterial;

    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    { return FPrimaryAssetId("MZSurface", SurfaceID); }
};
```

#### Source/MileZero/Surfaces/MZSurfaceContactComponent.h + .cpp
```cpp
// .h
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MZSurfaceContactComponent.generated.h"
class UMZSurfaceDataAsset;

UCLASS(ClassGroup = (MileZero), meta = (BlueprintSpawnableComponent))
class MILEZERO_API UMZSurfaceContactComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UMZSurfaceContactComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    UFUNCTION(BlueprintPure) const UMZSurfaceDataAsset* GetCurrentSurface() const { return CurrentSurface; }
    UFUNCTION(BlueprintPure) float GetCurrentGripMultiplier() const;
    UFUNCTION(BlueprintPure) FName GetCurrentSurfaceID() const;
protected:
    virtual void BeginPlay() override;
    UPROPERTY(EditAnywhere) TMap<FName, TObjectPtr<UMZSurfaceDataAsset>> SurfaceLookup;
    UPROPERTY(EditAnywhere) TObjectPtr<UMZSurfaceDataAsset> DefaultSurface;
private:
    void DetectSurface();
    UPROPERTY() TObjectPtr<const UMZSurfaceDataAsset> CurrentSurface;
};

// .cpp — trace line verso il basso, risolve PhysicalMaterial → SurfaceDataAsset via SurfaceLookup
```

### 6.7 Activities, Save, Debug

#### Source/MileZero/Activities/MZActivityDataAsset.h
```cpp
UENUM(BlueprintType)
enum class EMZActivityType : uint8 { CheckpointSprint, DeliveryRun, TrafficWeave, DamageSurvival };

UCLASS(BlueprintType)
class MILEZERO_API UMZActivityDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere) FName ActivityID;
    UPROPERTY(EditAnywhere) FText DisplayName;
    UPROPERTY(EditAnywhere) FText Description;
    UPROPERTY(EditAnywhere) EMZActivityType ActivityType = EMZActivityType::CheckpointSprint;
    UPROPERTY(EditAnywhere) float TimeLimitSeconds = 0.0f;
    UPROPERTY(EditAnywhere) float MaxDamageAllowed = 0.0f;
    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    { return FPrimaryAssetId("MZActivity", ActivityID); }
};
```

#### Source/MileZero/Save/MZSaveGame.h + .cpp
```cpp
UCLASS()
class MILEZERO_API UMZSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY() FName LastVehicleID;
    UPROPERTY() FTransform PlayerTransform;
    UPROPERTY() TArray<FName> CompletedActivities;
    static const FString DefaultSlotName; // = "MileZeroSave"
};
```

#### Source/MileZero/Debug/MZDebugSubsystem.h + .cpp
Subsystem con 4 comandi console registrati via IConsoleManager:
- `MZ.ResetVehicle` — reset veicolo al punto di spawn
- `MZ.SpawnVehicle <ID>` — spawn veicolo per ID (stub)
- `MZ.Teleport X Y Z` — teleporta pawn
- `MZ.Telemetry` — toggle overlay telemetria

### 6.8 Setup Commandlet

#### Source/MileZero/Setup/MZSetupCommandlet.h + .cpp
Commandlet eseguibile via: `UnrealEditor-Cmd.exe MileZero.uproject -run=MZSetup`

Crea automaticamente `L_MZ_TestTrack.umap` con 7 attori:
1. Floor (piano 200x200 scala)
2. PlayerStart (0, 0, 50)
3. DirectionalLight (sole, atmosphere-linked)
4. SkyAtmosphere
5. SkyLight (real-time capture)
6. ExponentialHeightFog
7. LightmassImportanceVolume

---

## 7. FILE DI CONFIGURAZIONE

### Config/DefaultEngine.ini
```ini
[/Script/EngineSettings.GameMapsSettings]
GameDefaultMap=/Game/MileZero/Maps/Test/L_MZ_TestTrack
EditorStartupMap=/Game/MileZero/Maps/Test/L_MZ_TestTrack
GlobalDefaultGameMode=/Script/MileZero.MZGameMode
GameInstanceClass=/Script/MileZero.MZGameInstance

[/Script/Engine.PhysicsSettings]
DefaultGravityZ=-980.0
bEnableCCD=True
bDefaultHasComplexCollision=True

[/Script/Engine.CollisionProfile]
+Profiles=(Name="MZVehicleBody",CollisionEnabled=QueryAndPhysics,ObjectTypeName="Vehicle",CustomResponses=())

[/Script/Engine.RendererSettings]
r.DefaultFeature.AutoExposure=True
r.DefaultFeature.MotionBlur=False

[/Script/Engine.Engine]
+ActiveGameNameRedirects=(OldGameName="TP_Blank",NewGameName="/Script/MileZero")

[/Script/HardwareTargeting.HardwareTargetingSettings]
TargetedHardwareClass=Desktop

[SystemSettings]
physics.vehicle.MZEnableDetailedLogging=1
```

### Config/DefaultGame.ini
```ini
[/Script/EngineSettings.GeneralProjectSettings]
ProjectID=811E88D1D5224DECBDC3E3F38A8F0E2B
ProjectName=MileZero
CompanyName=MileZero
Description=Physics-first driving sandbox

[/Script/Engine.AssetManagerSettings]
+PrimaryAssetTypesToScan=(PrimaryAssetType="Map",...)
+PrimaryAssetTypesToScan=(PrimaryAssetType="MZVehicle",AssetBaseClass=/Script/MileZero.MZVehicleDataAsset,...)
+PrimaryAssetTypesToScan=(PrimaryAssetType="MZSurface",AssetBaseClass=/Script/MileZero.MZSurfaceDataAsset,...)
+PrimaryAssetTypesToScan=(PrimaryAssetType="MZActivity",AssetBaseClass=/Script/MileZero.MZActivityDataAsset,...)
```

### Config/DefaultInput.ini
```ini
[/Script/Engine.InputSettings]
DefaultPlayerInputClass=/Script/EnhancedInput.EnhancedPlayerInput
DefaultInputComponentClass=/Script/EnhancedInput.EnhancedInputComponent

[/Script/EnhancedInput.EnhancedInputDeveloperSettings]
bEnableWorldSubsystem=True
```

---

## 8. SCRIPT E TOOL

### tools/compile_check.bat
Compilazione rapida: trova UE 5.7, esegue UBT per MileZeroEditor Win64 Development.

### tools/generate_project_files.bat
Genera file progetto VS tramite UBT `-projectfiles`.

### tools/open_editor.bat
Lancia `UnrealEditor.exe` su MileZero.uproject.

### tools/build.bat
Build completo via RunUAT: BuildCookRun con cook+stage+pak.

### tools/setup_editor_assets.py
Script Python per l'editor: crea input actions, mapping context, data asset veicolo/superficie, mappa test.

---

## 9. ASSET BINARI (CONTENT)

### Livello test
| File | Tipo | Note |
|------|------|------|
| Content/MileZero/Maps/Test/L_MZ_TestTrack.umap | Livello | Floor, PlayerStart, luci, atmosfera, fog, lightmass volume |
| Content/MileZero/Maps/Test/L_MZ_TestTrack_BuiltData.uasset | Dati illuminazione | Generato dall'editor |

### OffroadCar (da UE 5.7 Templates)
| File | Tipo |
|------|------|
| SKM_Offroad.uasset | Skeletal Mesh (bone: PhysWheel_FL/FR/BL/BR, VisWheel_FL/FR/BL/BR) |
| SKM_Offroad_Skeleton.uasset | Skeleton |
| SKM_Offroad_PhysicsAsset.uasset | Physics Asset |
| SM_Offroad_Body.uasset | Static Mesh carrozzeria |
| SM_Offroad_Tire.uasset | Static Mesh pneumatico |
| Offroad_AnimBP.uasset | Animation Blueprint |
| Offroad_CtrlRig.uasset | Control Rig |
| Materials/MI_OffroadCar_*.uasset | 6 Material Instance |
| Materials/Textures/T_OffroadCar_*.uasset | 13 Texture |

### SportsCar (da UE 5.7 Templates)
| File | Tipo |
|------|------|
| SKM_SportsCar.uasset | Skeletal Mesh |
| SK_SportsCar.uasset | Skeleton |
| PA_SportsCar.uasset | Physics Asset |
| SM_SportsCar.uasset, SM_SportsCar_Glass.uasset, SM_SportsCar_Wheel.uasset | Static Mesh |
| ABP_SportsCar.uasset | Animation Blueprint |
| Materials/*.uasset | 7 materiali |
| Materials/Textures/*.uasset | 11 texture |
| *.FBX | Sorgenti mesh originali |

### Physics Materials
| File | Tipo |
|------|------|
| NonSlippery.uasset | Physical Material |
| Slippery.uasset | Physical Material |

---

## 10. DOCUMENTAZIONE DI PROGETTO

Tutti i file in `docs/`:

| File | Contenuto |
|------|-----------|
| STATE.md | Stato corrente: M1 IN_PROGRESS, ultimo build PASS 2026-03-27 |
| TODO.md | Task: M1 quasi completo, manca solo PIE test |
| ROADMAP_EXECUTION.md | Milestone M0-M8 con gate, checklist, exit criteria, fallback |
| MILESTONE_STATUS.md | M0=VERIFIED, M1=IN_PROGRESS, M2-M8=NOT_STARTED |
| VERIFICATION.md | 14 entry di verifica dal 2026-03-16 al 2026-03-27 |
| DECISIONS.md | 7 decisioni: UE5+C++, mondo compatto, data-driven, spawn in veicolo, Chaos, PrimaryDataAsset, fallback |
| ARCHITECTURE.md | Moduli, source tree, content tree, relazioni classi |
| EDITOR_BOOTSTRAP.md | Guida: apri editor, premi Alt+P, verifica guida |
| BUILD_RUNBOOK.md | Installazione, comandi build, errori comuni |
| KNOWN_ISSUES.md | 7 issue (6 risolte, 1 aperta: riferimenti template .uasset) |
| ASSET_MANIFEST.md | 23 asset pianificati (tutti PLANNED) |
| TEST_MATRIX.md | 14 test case M1-M7 (tutti NOT_RUN) |
| RISK_REGISTER.md | 8 rischi aperti con mitigazione |
| PHYSICS_TUNING.md | Envelop target veicoli, seed valori grip superfici |
| PROJECT_VISION.md | Visione: guida credibile, mondo compatto, crash reali |

---

## 11. STATO CORRENTE E MILESTONE

### Milestone attuale: M1 — First drivable core
**Status**: IN_PROGRESS (codice completo, in attesa di PIE test)

### Cosa funziona senza NESSUN setup nell'editor:
- Il veicolo si auto-configura: mesh OffroadCar, 4 ruote, motore, trasmissione, sterzo
- Input auto-creati a runtime: W/S (throttle/brake), A/D (steer), Space (handbrake), C (camera), R (reset), Mouse (look), Gamepad
- Camera chase + hood con cycling
- Livello L_MZ_TestTrack generato con floor, PlayerStart, sole, cielo, fog
- GameMode configura il veicolo come pawn di default
- Console commands: MZ.ResetVehicle, MZ.SpawnVehicle, MZ.Teleport, MZ.Telemetry

### Per completare M1:
1. Aprire l'editor (`tools\open_editor.bat`)
2. Build > Build Lighting Only
3. Alt+P per giocare
4. Verificare: accelera, frena, sterza, freno a mano, camera, reset
5. Se tutto funziona → M1 = VERIFIED

### Milestone rimanenti:
| Milestone | Descrizione | Stato |
|-----------|-------------|-------|
| M0 | Bootstrap progetto | VERIFIED |
| M1 | Prima guida funzionante | IN_PROGRESS |
| M2 | Sistema superfici + telemetria | NOT_STARTED |
| M3 | Differenziazione 3 veicoli | NOT_STARTED |
| M4 | Danni meccanici | NOT_STARTED |
| M5 | Mondo greybox | NOT_STARTED |
| M6 | Attivita', HUD, save/load | NOT_STARTED |
| M7 | Traffico, giorno/notte, wet | NOT_STARTED |
| M8 | Hardening, performance | NOT_STARTED |

---

## 12. COME CONTINUARE LO SVILUPPO

### Per uno sviluppatore che apre il progetto per la prima volta:

1. **Requisiti**: UE 5.7 + VS 2022/2026 con C++ Desktop workload
2. **Compilare**: `tools\compile_check.bat`
3. **Creare livello test** (se non esiste): `UnrealEditor-Cmd.exe MileZero.uproject -run=MZSetup`
4. **Aprire editor**: `tools\open_editor.bat` o doppio click su MileZero.uproject
5. **Build Lighting**: Build > Build Lighting Only
6. **Testare**: Alt+P — il veicolo si guida con WASD + Space + mouse
7. **Leggere**: docs/ROADMAP_EXECUTION.md per i prossimi step

### Architettura chiave da capire:
- `AMZVehiclePawn` e' il cuore del progetto — veicolo Chaos auto-configurante
- `UMZVehicleDataAsset` definisce le spec di ogni veicolo (data-driven)
- `UMZSurfaceContactComponent` rileva la superficie sotto il veicolo
- `AMZGameMode` spawna il giocatore direttamente nel veicolo (M1 fallback)
- Input Enhanced si auto-creano se nessun asset editor e' assegnato

### Priorita' di sviluppo (dal contratto):
1. Guidabilita' immediata
2. Handling differenziato e credibile
3. Comportamento superfici
4. Conseguenze danni
5. Un loop di attivita' completo
6. Traffico AI
7. Save/load
8. Densita' mondo
9. UX polish
10. Fedelta' cosmetica

---

*Fine documento. Ultima modifica: 2026-03-27.*
