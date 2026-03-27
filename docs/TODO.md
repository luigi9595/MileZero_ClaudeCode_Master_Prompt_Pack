# TODO

## Now (M8 — Hardening)
- [x] M0: Bootstrap progetto
- [x] M1: Veicolo guidabile con input, camera, reset
- [x] M2: Sistema superfici + telemetria HUD + gameplay HUD
- [x] M3: 3 veicoli differenziati (Coupe/Hatch/Pickup) + registry + selezione
- [x] M4: Danni meccanici (sterzo, motore, sospensioni, raffreddamento, aero, freni, ruote)
- [x] M5: Infrastruttura mondo (zone, spawn point, world manager, commandlet)
- [x] M6: Activity manager + checkpoint/delivery actors + save/load
- [x] M7: Traffico AI + ciclo giorno/notte + meteo/wet roads
- [x] M8: Wiring finale — damage↔vehicle↔surface↔weather nel VehiclePawn::Tick
- [ ] **COMPILE**: eseguire `tools\compile_check.bat` su Windows
- [ ] **FIX ERRORS**: risolvere eventuali errori di compilazione
- [ ] **PIE TEST**: Alt+P, guidare, verificare ogni sistema
- [ ] **VERIFY**: loggare risultati in docs/VERIFICATION.md

## Checklist verifica PIE
- [ ] Il veicolo si muove con WASD + Space + Mouse
- [ ] Camera chase e hood funzionano (C per ciclare)
- [ ] Reset veicolo (R) riporta allo spawn
- [ ] MZ.Telemetry mostra overlay debug (speed, RPM, gear, surface, grip, FPS)
- [ ] HUD gameplay mostra speed/RPM/gear in basso
- [ ] MZ.SpawnVehicle Coupe / Hatch / Pickup cambia veicolo
- [ ] MZ.ListVehicles stampa i 3 profili
- [ ] Superficie diversa = grip diverso (se physical materials assegnati)
- [ ] Collisione a >15 km/h causa danni visibili in telemetria
- [ ] Reset (R) ripara i danni

## Deferred (post-v1)
- [ ] Personaggio a piedi con enter/exit veicolo
- [ ] UI menu principale
- [ ] Audio motore/collisioni
- [ ] Effetti visivi danni carrozzeria
- [ ] Particle effects per superfici
- [ ] Minimap
