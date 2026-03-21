# KNOWN_ISSUES

| ID | Issue | Severity | Workaround | Status |
|---|---|---|---|---|
| KI-001 | Chaos Vehicle API property names in ApplyVehicleData() may not match exact UE 5.5 headers | Medium | Fix targeted property names against actual header at first compile | OPEN |
| KI-002 | Target.cs uses BuildSettingsVersion.V5 / Unreal5_5 — breaks if UE 5.4 installed | Low | Change to V4 / Unreal5_4 if needed | OPEN |
| KI-003 | ChaosVehiclesCore in Build.cs PrivateDependency may not be linkable | Low | Remove from PrivateDependencyModuleNames if linker error | OPEN |
| KI-004 | WheeledVehiclePawn.h include path may need module prefix | Low | Change to ChaosVehicles/WheeledVehiclePawn.h if compile error | OPEN |
| KI-005 | Vehicle pawn requires skeletal mesh with wheel bones to simulate | High | Assign any vehicle skeletal mesh during editor setup | OPEN |

## Detail

### KI-001: Chaos Vehicle API property access
`MZVehiclePawn.cpp:ApplyVehicleData()` directly accesses:
- `Movement->EngineSetup.MaxTorque` etc.
- `Movement->TransmissionSetup.FinalRatio`, `ForwardGearRatios`
- `Movement->SteeringSetup.MaxSteeringAngle` — may be per-wheel, not on steering config
- `Movement->CenterOfMassOverride` / `bEnableCenterOfMassOverride`
- `Movement->DragCoefficient`

If any fail: check `UChaosWheeledVehicleMovementComponent` and `UChaosVehicleMovementComponent` headers for actual property names. Fix is always a targeted rename, not a rewrite.

### KI-005: Skeletal mesh requirement
`AWheeledVehiclePawn` requires a `USkeletalMeshComponent` with wheel bones for Chaos simulation. Without it, the vehicle won't have physical wheels and may fall through the ground or not respond to input. A placeholder mesh (even from UE5 vehicle template content) is essential.
