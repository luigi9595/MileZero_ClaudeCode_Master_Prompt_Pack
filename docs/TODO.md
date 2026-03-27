# TODO

## Now (M1 — First drivable core)
- [x] ~~install toolchain~~ UE 5.7 + VS 2026 confirmed
- [x] ~~compile project~~ PASS on 2026-03-27
- [x] ~~input actions~~ auto-bootstrap at runtime (no editor assets needed)
- [x] ~~test level~~ L_MZ_TestTrack.umap created via MZSetup commandlet
- [x] ~~vehicle template content~~ OffroadCar + SportsCar copied to Content/Vehicles/
- [ ] create `BP_MZVehicle_Hatch` Blueprint (assign mesh + wheel setups)
- [ ] set GameMode default pawn to Blueprint
- [ ] PIE test: throttle, brake, steer, handbrake, camera, reset

## Next (after M0+M1 verified)
- [ ] wire UMZSurfaceContactComponent to vehicle pawn
- [ ] verify dry asphalt vs gravel difference in feel (M2 first gate)
- [ ] expose telemetry (speed, RPM, gear, surface) via HUD or debug print (M2 second gate)

## Later
- [ ] create DA_MZVehicle_Coupe and DA_MZVehicle_Pickup (M3)
- [ ] build telemetry HUD overlay WBP_MZTelemetry (M2 full)
- [ ] implement UMZVehicleDamageComponent (M4)
- [ ] build main world greybox (M5)

## Deferred
- [ ] on-foot character enter/exit vehicle flow (stubbed, clean interface exists)
- [ ] traffic AI (M7)
- [ ] day/night cycle (M7)
- [ ] save/load persistence (M6)
