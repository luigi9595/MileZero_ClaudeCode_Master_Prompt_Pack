# KNOWN_ISSUES

| ID | Issue | Severity | Workaround | Status |
|---|---|---|---|---|
| KI-001 | Chaos Vehicle API property names | Medium | Fixed: SteeringSetup.MaxSteeringAngle → SetWheelMaxSteerAngle(), GetEngineRotationSpeed() cast | RESOLVED |
| KI-002 | Target.cs build settings version | Low | Fixed: updated to V6 / Unreal5_7 for UE 5.7 | RESOLVED |
| KI-003 | ChaosVehiclesCore PrivateDependency | Low | Compiled and linked successfully | RESOLVED |
| KI-004 | WheeledVehiclePawn.h include path | Low | ChaosWheeledVehicleMovementComponent.h found by default include paths | RESOLVED |
| KI-005 | Vehicle pawn requires skeletal mesh with wheel bones to simulate | High | Assign any vehicle skeletal mesh during editor setup | OPEN |
| KI-006 | V6 build settings require explicit include paths for subdirectories | Low | Fixed: PrivateIncludePaths.Add(ModuleDirectory) in Build.cs | RESOLVED |

## Detail

### KI-005: Skeletal mesh requirement
`AWheeledVehiclePawn` requires a `USkeletalMeshComponent` with wheel bones for Chaos simulation. Without it, the vehicle won't have physical wheels and may fall through the ground or not respond to input. A placeholder mesh (even from UE5 vehicle template content) is essential.
