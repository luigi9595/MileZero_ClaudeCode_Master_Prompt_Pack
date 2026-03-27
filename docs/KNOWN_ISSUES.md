# KNOWN_ISSUES

| ID | Issue | Severity | Workaround | Status |
|---|---|---|---|---|
| KI-001 | Chaos Vehicle API property names | Medium | Fixed: SteeringSetup.MaxSteeringAngle → SetWheelMaxSteerAngle(), GetEngineRotationSpeed() cast | RESOLVED |
| KI-002 | Target.cs build settings version | Low | Fixed: updated to V6 / Unreal5_7 for UE 5.7 | RESOLVED |
| KI-003 | ChaosVehiclesCore PrivateDependency | Low | Compiled and linked successfully | RESOLVED |
| KI-004 | WheeledVehiclePawn.h include path | Low | ChaosWheeledVehicleMovementComponent.h found by default include paths | RESOLVED |
| KI-005 | Vehicle pawn requires skeletal mesh with wheel bones to simulate | High | Fixed: constructor auto-loads SKM_Offroad + configures PhysWheel_FL/FR/BL/BR | RESOLVED |
| KI-006 | V6 build settings require explicit include paths for subdirectories | Low | Fixed: PrivateIncludePaths.Add(ModuleDirectory) in Build.cs | RESOLVED |

## Detail

### KI-005: Skeletal mesh requirement (RESOLVED)
Vehicle pawn constructor now auto-loads `SKM_Offroad` from `Content/Vehicles/OffroadCar/` and configures 4 wheels with `MZWheelFront`/`MZWheelRear` classes and bone names `PhysWheel_FL/FR/BL/BR`. If the template content fails to load (broken references), add the Vehicle content pack from the editor.

### KI-007: Copied template .uasset files may have broken internal references
Vehicle template content was file-copied from `UE_5.7/Templates/TemplateResources/Standard/Vehicles/`. If cross-asset references break, use the editor's "Add Feature or Content Pack" > Vehicle instead.
