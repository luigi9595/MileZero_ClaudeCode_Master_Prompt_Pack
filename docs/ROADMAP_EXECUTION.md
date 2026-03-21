# ROADMAP_EXECUTION

## Current milestone
- Current milestone: M0
- Current status: BLOCKED
- Current gate: compile verification — no UE5 or VS2022 on this machine (confirmed 2026-03-16)
- Blocker: hard external — Epic Games Launcher, Unreal Engine 5.x, and Visual Studio 2022 must be installed
- Owner: User (install) → Claude (compile + advance)

## Milestone status legend
- NOT_STARTED
- IN_PROGRESS
- BLOCKED
- VERIFIED
- DEFERRED

---

## M0 — Project bootstrap and repo truth
**Goal:** Establish a truthful, build-oriented project skeleton and persistent project memory.

### Mandatory outputs
- [x] MileZero UE5 project exists or strongest feasible text-side skeleton exists
- [x] runtime module scaffolding exists
- [x] required docs exist and are meaningful
- [x] repo structure exists
- [x] source folders exist
- [x] content folder plan exists
- [x] asset manifest exists
- [x] roadmap docs exist
- [x] decisions captured

### Technical checklist
- [x] inspect workspace and tools
- [x] detect engine / build / editor availability
- [x] create or adapt MileZero project
- [x] create docs listed in CLAUDE.md
- [x] create source layout
- [x] create tools folder
- [x] create initial BUILD_RUNBOOK.md if commands are known
- [x] create EDITOR_BOOTSTRAP.md if editor asset generation is unavailable

### Verification gates
- [x] nearest available project-generation / build / validation step attempted
- [x] result logged in docs/VERIFICATION.md
- [x] docs/STATE.md and docs/TODO.md updated
- [x] current blockers recorded precisely if any

### Exit criteria
- [x] repo is organized enough that the next session can continue immediately
- [x] there is a clear current milestone and next gate
- [x] repo truth is documented

### Fallback if blocked
- [x] create full text-side scaffolding
- [x] create editor bootstrap instructions
- [x] create build runbook placeholders
- [x] document exact blockers and continue toward M1 where possible

---

## M1 — First drivable core
**Goal:** Get a real drivable gameplay loop working as early as possible.

### Mandatory outputs
- [ ] Enhanced Input pipeline established
- [ ] AMZGameMode / AMZPlayerController / AMZCharacter baseline exists
- [ ] enter / exit vehicle path exists or strongest usable stub exists
- [ ] AMZVehiclePawn exists
- [ ] L_MZ_TestTrack exists or exact asset/bootstrap path exists
- [ ] one drivable hatchback exists
- [ ] chase camera works
- [ ] reset / recovery flow exists

### Technical checklist
- [ ] input contexts / actions wired
- [ ] player spawn path defined
- [ ] vehicle possession path defined
- [ ] throttle / brake / steer / handbrake work
- [ ] camera follow implemented
- [ ] test spawn point exists
- [ ] simple greybox track or pad exists

### Verification gates
- [ ] nearest build / compile attempted and logged
- [ ] can enter or start in vehicle
- [ ] vehicle can accelerate, steer, brake, and recover / reset
- [ ] current map or fallback bootstrap path documented

### Exit criteria
- [ ] first drivable loop exists
- [ ] a user can spawn and drive immediately

### Fallback if blocked
- [ ] keep the player starting inside the vehicle
- [ ] stub the enter / exit path behind a clean interface
- [ ] create test-map bootstrap instructions
- [ ] document the narrowest missing dependency

---

## M2 — Surface system and telemetry
**Goal:** Make driving readable, tunable, and measurably different across surfaces.

### Mandatory outputs
- [ ] surface profiles exist for dry / wet / gravel / grass
- [ ] UMZSurfaceContactComponent exists
- [ ] telemetry / debug overlay exists or strongest clean stub exists
- [ ] test track contains or documents distinct surface zones
- [ ] PHYSICS_TEST_PLAN.md and TEST_MATRIX.md initialized

### Technical checklist
- [ ] detect current surface
- [ ] apply surface effect to handling
- [ ] expose speed, RPM, gear, inputs, current surface
- [ ] add debug commands or overlay toggles
- [ ] create first tuning notes

### Verification gates
- [ ] dry vs gravel difference is noticeable and logged
- [ ] telemetry shows speed, RPM, gear, throttle, brake, steer, surface
- [ ] verification entry includes at least one surface comparison result

### Exit criteria
- [ ] the project is tunable by evidence rather than guesswork

### Fallback if blocked
- [ ] expose surface identity and pending modifiers in telemetry
- [ ] document exact missing asset hook or editor-only dependency
- [ ] keep tuning data assets moving forward

---

## M3 — Vehicle trio differentiation
**Goal:** Make the three vehicles feel materially different.

### Mandatory outputs
- [ ] DA_MZVehicle_Coupe
- [ ] DA_MZVehicle_Hatch
- [ ] DA_MZVehicle_Pickup
- [ ] selection / spawn path for all three vehicles
- [ ] differentiated tuning profiles
- [ ] measurement notes per vehicle

### Technical checklist
- [ ] different mass and drivetrain behavior
- [ ] different steering behavior
- [ ] different brake feel
- [ ] different bump response
- [ ] different rotation tendencies
- [ ] quick-select spawn / debug method

### Verification gates
- [ ] all three vehicles compile / load through the chosen path
- [ ] differences documented in TEST_MATRIX.md
- [ ] at least two measurable or clearly observed differences per vehicle logged

### Exit criteria
- [ ] switching vehicles clearly changes the driving experience

### Fallback if blocked
- [ ] keep one common pawn but different specs
- [ ] fake selection via debug spawn until final UI exists
- [ ] document asset dependency blockers precisely

---

## M4 — Mechanical damage consequences
**Goal:** Make collisions matter mechanically, not just visually.

### Mandatory outputs
- [ ] impact sampling or collision interpretation exists
- [ ] UMZVehicleDamageComponent exists
- [ ] damage profile data exists
- [ ] light / medium / heavy damage states exist
- [ ] HUD or telemetry exposes damage state
- [ ] reset / repair / recover flow exists

### Technical checklist
- [ ] steering pull or alignment degradation
- [ ] suspension degradation
- [ ] engine health / power loss
- [ ] cooling / overheat behavior or strongest usable approximation
- [ ] drag / top-speed penalty
- [ ] brake stability penalty
- [ ] damaged-corner traction penalty

### Verification gates
- [ ] at least three distinct impact severities logged
- [ ] post-crash drivability changes are observable and recorded
- [ ] damage state is surfaced in telemetry or HUD
- [ ] recovery / reset path is testable

### Exit criteria
- [ ] crashes materially change driving behavior

### Fallback if blocked
- [ ] start with alignment + power + drag penalties
- [ ] leave clean hooks for deeper suspension damage later
- [ ] keep damage data-driven

---

## M5 — Main world greybox
**Goal:** Create the compact, dense world structure for replayable free-roam driving.

### Mandatory outputs
- [ ] L_MZ_MainWorld exists or strongest documented bootstrap path exists
- [ ] city district greybox
- [ ] industrial zone greybox
- [ ] highway / ring-road section
- [ ] rural / gravel spur
- [ ] proving ground
- [ ] spawn points and recovery zones
- [ ] activity anchors placed or documented

### Technical checklist
- [ ] world partition path established if available
- [ ] data layers created or documented
- [ ] route loops and shortcuts designed
- [ ] proving ground supports repeatable tests
- [ ] drive routes between all major zones exist

### Verification gates
- [ ] can drive between all core zones or documented near-term equivalent exists
- [ ] world density is intentional, not giant and empty
- [ ] verification notes include route / readability observations

### Exit criteria
- [ ] free roam is enjoyable even in greybox form

### Fallback if blocked
- [ ] keep separate greybox test maps but unify route plan in docs
- [ ] document exact world-partition/editor limitation
- [ ] continue with activities using test-track variants

---

## M6 — Activities, HUD, and save / load
**Goal:** Turn the sandbox into a structured playable loop.

### Mandatory outputs
- [ ] AMZActivityManager exists
- [ ] activity definitions exist for all 4 named activities
- [ ] checkpoint sprint playable
- [ ] delivery run playable
- [ ] HUD exists
- [ ] pause / menu flow exists
- [ ] save / load baseline exists

### Technical checklist
- [ ] activity start flow
- [ ] objective text
- [ ] success / fail conditions
- [ ] reset flow
- [ ] progression hooks
- [ ] vehicle and player state persistence
- [ ] current activity surfaced on HUD

### Verification gates
- [ ] at least 2 activities work end-to-end and are logged
- [ ] save / load retains core state and is logged
- [ ] HUD exposes speed, RPM, gear, objective, damage

### Exit criteria
- [ ] structured gameplay loop exists beyond free roam

### Fallback if blocked
- [ ] keep activities started from debug actors or console commands
- [ ] keep save/load narrow and extend later
- [ ] document missing UI asset steps precisely

---

## M7 — Traffic, day/night, wet-world integration, activity completion
**Goal:** Integrate systemic world behavior and complete the remaining activity loops.

### Mandatory outputs
- [ ] traffic subsystem exists
- [ ] traffic spawner exists
- [ ] traffic AI controller / path behavior exists
- [ ] day / night cycle exists
- [ ] wet-road behavior exists in the world
- [ ] remaining activities completed to usable state

### Technical checklist
- [ ] lane / path follow
- [ ] speed variance
- [ ] stop / yield
- [ ] lightweight avoidance
- [ ] simple congestion recovery
- [ ] wet sections affect handling in world
- [ ] traffic weaving challenge playable
- [ ] damage survival event playable

### Verification gates
- [ ] traffic works in at least one district and one fast-road segment
- [ ] day / night or time-of-day change is testable
- [ ] wet vs dry difference is documented in-world
- [ ] all 4 activities are usable and logged

### Exit criteria
- [ ] the world feels systemic, not static

### Fallback if blocked
- [ ] keep traffic on a narrow demo corridor
- [ ] use simplified path actors before full lane graphs
- [ ] document time-of-day limits and keep world progression moving

---

## M8 — Hardening, performance, and release-candidate pass
**Goal:** Make the vertical slice credible, stable, and easy to continue.

### Mandatory outputs
- [ ] BUILD_RUNBOOK.md updated
- [ ] KNOWN_ISSUES triaged
- [ ] settings persistence reviewed
- [ ] performance notes recorded
- [ ] docs synchronized with repo reality
- [ ] packaging or near-packaging path documented

### Technical checklist
- [ ] smoke-run checklist
- [ ] obvious crashes / blockers addressed
- [ ] debug commands reviewed
- [ ] asset manifest updated
- [ ] next-phase roadmap added

### Verification gates
- [ ] smoke test run and logged
- [ ] performance observations logged
- [ ] outstanding blockers clearly classified
- [ ] next-step roadmap exists

### Exit criteria
- [ ] repo is handoff-ready and continuation-ready

### Fallback if blocked
- [ ] document the exact packaging/build blocker
- [ ] keep smoke tests and runbook current
- [ ] leave the repo in a continuation-ready state
