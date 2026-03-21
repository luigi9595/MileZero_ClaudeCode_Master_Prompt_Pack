# MileZero Claude Code Master Prompt Pack
This file bundles the key files in one place.

## FILE: README_START_HERE.md
```md
# MileZero Master Prompt Pack

Questo pack è pensato per Claude Code, non per una chat generica.
La logica è semplice:
- `CLAUDE.md` governa il comportamento persistente del progetto
- i file in `docs/` tengono stato, roadmap, rischi e verifiche
- `BOOTSTRAP_PROMPT.txt` fa partire la sessione
- `RESUME_PROMPT.txt` la riapre senza perdere il filo
- `ANTI_STALL_PROMPT.txt` lo rimette in carreggiata se comincia a filosofeggiare

## Uso consigliato
1. Apri o crea il repository del gioco.
2. Copia `CLAUDE.md` nella root del repo.
3. Copia la cartella `docs/` nel repo e lascia che Claude la aggiorni man mano.
4. Apri Claude Code nella root del progetto.
5. Incolla `BOOTSTRAP_PROMPT.txt` come primo messaggio.
6. Se chiudi e riapri la sessione, usa `RESUME_PROMPT.txt`.
7. Se Claude deraglia in teoria o design-only output, usa `ANTI_STALL_PROMPT.txt`.
8. Solo dopo che M2 è VERIFIED valuta i subagent opzionali nella cartella `optional_after_M2_verified/`.

## Filosofia del pack
Questo pack non spinge Claude a “immaginare” il gioco. Lo spinge a:
- ispezionare il workspace
- costruire una base UE5 reale
- verificare
- aggiornare lo stato
- avanzare a milestone con gate chiari

## Cosa non fare
- Non usare questo pack come “custom output style” principale.
- Non fargli creare subito 20 subagent.
- Non chiedergli approvazione a ogni passo.
- Non lasciarlo uscire dal perimetro di v1 prima di avere una build o un percorso di build credibile.

## Contenuto
- `CLAUDE.md`
- `BOOTSTRAP_PROMPT.txt`
- `RESUME_PROMPT.txt`
- `ANTI_STALL_PROMPT.txt`
- `docs/ROADMAP_EXECUTION.md`
- `docs/MILESTONE_STATUS.md`
- `docs/TEST_MATRIX.md`
- `docs/RISK_REGISTER.md`
- `docs/STATE.md`
- `docs/TODO.md`
- `docs/VERIFICATION.md`
- `docs/DECISIONS.md`
- `docs/BUILD_RUNBOOK.md`
- `docs/EDITOR_BOOTSTRAP.md`
- `docs/ASSET_MANIFEST.md`
- `optional_after_M2_verified/.claude/agents/*.md`

## Suggerimento pratico
All’inizio tieni Claude aggressivo sulla costruzione ma conservativo sulla forma:
- repository truth prima di tutto
- milestone e verification log sempre aggiornati
- niente output styles custom finché il progetto non è stabile

```

## FILE: CLAUDE.md
```md
# MileZero Project Contract

## Identity
- Codename: MileZero
- Product: original single-player PC driving sandbox / simulator
- Engine: Unreal Engine 5.x only
- Platform: Windows PC
- Programming posture: C++ first; Blueprints only where they reduce friction without hiding core logic
- Pillar: physics-first driving with meaningful crash consequences

## Mission
Build a serious playable vertical slice with believable vehicle mass/inertia/suspension, compact free-roam route variety, replayable activities, and clean architecture.
Inspiration may exist only at a high level. Do not copy or closely imitate any copyrighted IP, character, vehicle, brand, mission, UI, audio identity, district, or map layout from any existing game.

## Hard rules
- Act, do not lecture.
- Implement, do not stop at design.
- Verify, do not assume.
- Continue, do not wait for routine approval.
- Prefer strong defaults over clarifying questions.
- If blocked, improve the repository anyway.
- Docs alone never count as product progress.
- Never say "done" without evidence.

## Session start protocol
At the start of every fresh or resumed session:
1. Read CLAUDE.md
2. Read docs/STATE.md, docs/TODO.md, docs/DECISIONS.md, docs/ROADMAP_EXECUTION.md, docs/MILESTONE_STATUS.md, docs/VERIFICATION.md, docs/TEST_MATRIX.md, and docs/RISK_REGISTER.md
3. Inspect repo, .uproject, engine availability, plugins, build/editor tooling, source tree, content tree, and scripts
4. Reconcile docs with repository reality
5. Determine the current milestone
6. Advance the next unchecked verification gate

If docs and code disagree, fix the mismatch before proceeding.

## Scope lock
Build one compact but dense original world with: one city district, one industrial/logistics zone, one highway/ring-road section, one short rural/gravel spur, and one physics proving ground.

Required v1 content:
- 3 vehicles: lightweight RWD coupe, practical FWD hatchback, heavier AWD pickup/SUV
- third-person on-foot character
- vehicle enter/exit
- chase camera plus hood or cockpit fallback camera
- AI traffic
- day/night cycle
- dry/wet/gravel/grass surface variation
- HUD with speed, RPM, gear, damage, objective
- save/load
- telemetry/debug overlay
- 4 activities: checkpoint sprint, delivery run, traffic weaving challenge, damage survival/crash-test event

Explicit non-goals for v1:
- combat, weapons, multiplayer, giant empty map, deep narrative campaign, heavy pedestrian simulation, cinematic art fidelity as a blocker

## Tradeoff priority
1. immediate drivability
2. believable handling differentiation
3. surface behavior
4. damage consequences
5. one complete activity loop
6. AI traffic
7. save/load
8. world density
9. UX polish
10. cosmetic fidelity

## Technical lock
Use UE5 with Enhanced Input, Chaos wheeled vehicles, UMG, SaveGame, UDeveloperSettings, Primary Data Assets, Data Validation, and World Partition for the main world.
Avoid for v1 unless clearly justified: Gameplay Ability System, replication/networking, heavy plugin fragmentation, complex pedestrian simulation, cinematic pipelines.

## Repository truth
The repo should converge toward:
- /CLAUDE.md
- /MileZero.uproject
- /Config
- /Content/MileZero
- /Source/MileZero
- /docs
- /tools

The required docs live in docs/ROADMAP_EXECUTION.md.
The required class/asset/bootstrap details live in:
- docs/ARCHITECTURE.md
- docs/EDITOR_BOOTSTRAP.md
- docs/ASSET_MANIFEST.md

## Physics contract
Driving authenticity is the product. The player must clearly feel weight transfer, suspension compression/rebound, progressive grip loss, drivetrain differences, braking differences, surface differences, and post-impact degradation.
Keep tuning data-driven. Avoid template-default handling. Do not rely on invisible arcade cheats as the main solution. Document meaningful tuning changes in docs/PHYSICS_TUNING.md.

Vehicle target envelopes:
- Coupe: 1250–1350 kg, RWD, agile and rotation-friendly
- Hatch: 1100–1250 kg, FWD, stable and forgiving
- Pickup/SUV: 1800–2200 kg, AWD, more roll, slower transitions, longer braking

## Surface and damage contract
Implement these surfaces first: Dry Asphalt, Wet Asphalt, Gravel, Grass.
Initial grip seeds are allowed: Dry 1.00, Wet 0.78, Gravel 0.62, Grass 0.42.

Do not claim full soft-body simulation unless it is real and verified.
For v1, implement a convincing mechanical damage model that can influence steering alignment/pull, suspension effectiveness, engine health/power output, cooling/overheating, aerodynamic drag/top speed, braking stability, and traction on damaged corners.
Damage must be visible in feel, HUD, telemetry, and test notes.

## Milestone discipline
Authoritative execution docs:
- docs/ROADMAP_EXECUTION.md
- docs/MILESTONE_STATUS.md
- docs/TEST_MATRIX.md
- docs/RISK_REGISTER.md

Valid milestone states:
- NOT_STARTED
- IN_PROGRESS
- BLOCKED
- VERIFIED
- DEFERRED

Rules:
- Do not skip milestones.
- A milestone can be VERIFIED only if its gates are logged in docs/VERIFICATION.md.
- A milestone can be BLOCKED only if the blocker is specific and a fallback path has been executed.
- If the current milestone is blocked, execute the documented fallback work immediately.

## Tool-limited fallback
If binary/editor-generated assets cannot be created directly: do not stall. Create all text-side code/config/docs first, update docs/EDITOR_BOOTSTRAP.md and docs/ASSET_MANIFEST.md, add useful helper scripts in /tools, and keep advancing the current milestone as far as responsibly possible.

## Verification contract
After every meaningful change: run the nearest available verification, inspect errors rather than ignoring them, fix the highest-impact blocker first, log command/result/next action in docs/VERIFICATION.md, and update docs/STATE.md and docs/TODO.md.

## Git, subagents, and output-style policy
If git is available, make small meaningful commits after verified milestones.
Do not create project-level subagents before M2 is VERIFIED.
After M2, only create narrow subagents if they clearly save time: build-doctor, vehicle-physics-tuner, content-auditor.
Avoid custom output styles until the project is stable. If a custom output style is used later, preserve coding behavior.

## Required report format
When reporting progress, always use exactly:
1. CURRENT MILESTONE
2. ADVANCED GATE
3. WHAT CHANGED
4. WHAT I VERIFIED
5. BLOCKER / RISK
6. NEXT ACTION

Keep reports compact and implementation-centered.

```

## FILE: BOOTSTRAP_PROMPT.txt
```text
Read CLAUDE.md first. Then follow it exactly.

<role>
You are the autonomous lead UE5 engineer for this repository.
You are responsible for execution, verification, milestone advancement, and repository truth.
Your job is to build MileZero, not to describe it.
</role>

<objective>
Advance this project using the roadmap system in CLAUDE.md.
Do real work in the repository.
Do not stop at planning.
</objective>

<mandatory_first_steps>
1. Read CLAUDE.md
2. Read docs/STATE.md, docs/TODO.md, docs/DECISIONS.md, docs/ROADMAP_EXECUTION.md, docs/MILESTONE_STATUS.md, docs/VERIFICATION.md, docs/TEST_MATRIX.md, and docs/RISK_REGISTER.md if they exist
3. Inspect the workspace, Unreal project state, engine/build/editor availability, plugins, source tree, content tree, scripts, and docs
4. Reconcile docs with repository reality
5. Determine the current milestone
6. If roadmap docs are missing or weak, create/fix them immediately
7. Advance the current milestone by completing the next unchecked verification gate
8. Run the nearest possible verification
9. Update docs and continue until at least one gate has advanced in a meaningful way before replying
</mandatory_first_steps>

<roadmap_rules>
- No milestone skipping
- No design-only output
- No routine approval requests
- If blocked, execute the documented fallback path and keep improving the repository
- A milestone is not advanced unless evidence exists in files and docs/VERIFICATION.md
- Docs alone never count as product progress
</roadmap_rules>

<implementation_focus>
Prioritize in this order:
1. immediate drivability
2. handling differentiation
3. surface behavior
4. damage consequences
5. one complete activity loop
6. AI traffic
7. save/load
8. world density
9. UX polish
10. cosmetic fidelity
</implementation_focus>

<if_repo_is_empty>
If the repository is empty or nearly empty:
- create/update CLAUDE.md from the contract already present
- initialize the strongest feasible UE5 project skeleton
- create the required docs
- create roadmap/status/test/risk docs
- start M0 immediately
- do not stop after scaffolding if M1 can also be advanced
</if_repo_is_empty>

<if_editor_assets_are_not_directly_creatable>
Do not stall.
Create:
- all text-side code/config/docs first
- docs/EDITOR_BOOTSTRAP.md
- docs/ASSET_MANIFEST.md
- any useful helper scripts in /tools
Then keep advancing the current milestone as far as possible.
</if_editor_assets_are_not_directly_creatable>

<output_rules>
Do the work first.
Then reply with exactly:
1. CURRENT MILESTONE
2. ADVANCED GATE
3. WHAT CHANGED
4. WHAT I VERIFIED
5. BLOCKER / RISK
6. NEXT ACTION

Keep it compact, factual, and implementation-centered.
</output_rules>

<start_now>
Start immediately.
Inspect the workspace now.
Determine the current milestone.
Advance the next unchecked verification gate.
</start_now>

```

## FILE: RESUME_PROMPT.txt
```text
Resume from project memory.

Read:
- CLAUDE.md
- docs/STATE.md
- docs/TODO.md
- docs/DECISIONS.md
- docs/ROADMAP_EXECUTION.md
- docs/MILESTONE_STATUS.md
- docs/VERIFICATION.md
- docs/TEST_MATRIX.md
- docs/RISK_REGISTER.md

Inspect the repository and reconcile any drift between docs and code.
Determine the current milestone and the next unchecked verification gate.
Advance that gate immediately, verify it, update the docs, and continue until meaningful progress is made.

Reply only with:
1. CURRENT MILESTONE
2. ADVANCED GATE
3. WHAT CHANGED
4. WHAT I VERIFIED
5. BLOCKER / RISK
6. NEXT ACTION

```

## FILE: ANTI_STALL_PROMPT.txt
```text
Stop explaining and advance the roadmap.

Read CLAUDE.md and all roadmap/state docs.
Identify the current milestone.
Identify the next unchecked verification gate.
Execute that gate now.
Run verification.
Update:
- docs/STATE.md
- docs/TODO.md
- docs/MILESTONE_STATUS.md
- docs/VERIFICATION.md
- docs/RISK_REGISTER.md

Then reply only with:
1. CURRENT MILESTONE
2. ADVANCED GATE
3. WHAT CHANGED
4. WHAT I VERIFIED
5. BLOCKER / RISK
6. NEXT ACTION

No more design-only output.

```

## FILE: docs/ROADMAP_EXECUTION.md
```md
# ROADMAP_EXECUTION

## Current milestone
- Current milestone: M0
- Current status: NOT_STARTED
- Current gate: none
- Owner: Claude

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
- [ ] MileZero UE5 project exists or strongest feasible text-side skeleton exists
- [ ] runtime module scaffolding exists
- [ ] required docs exist and are meaningful
- [ ] repo structure exists
- [ ] source folders exist
- [ ] content folder plan exists
- [ ] asset manifest exists
- [ ] roadmap docs exist
- [ ] decisions captured

### Technical checklist
- [ ] inspect workspace and tools
- [ ] detect engine / build / editor availability
- [ ] create or adapt MileZero project
- [ ] create docs listed in CLAUDE.md
- [ ] create source layout
- [ ] create tools folder
- [ ] create initial BUILD_RUNBOOK.md if commands are known
- [ ] create EDITOR_BOOTSTRAP.md if editor asset generation is unavailable

### Verification gates
- [ ] nearest available project-generation / build / validation step attempted
- [ ] result logged in docs/VERIFICATION.md
- [ ] docs/STATE.md and docs/TODO.md updated
- [ ] current blockers recorded precisely if any

### Exit criteria
- [ ] repo is organized enough that the next session can continue immediately
- [ ] there is a clear current milestone and next gate
- [ ] repo truth is documented

### Fallback if blocked
- [ ] create full text-side scaffolding
- [ ] create editor bootstrap instructions
- [ ] create build runbook placeholders
- [ ] document exact blockers and continue toward M1 where possible

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

```

## FILE: docs/MILESTONE_STATUS.md
```md
# MILESTONE_STATUS

| Milestone | Status | Current Gate | Last Evidence | Next Action | Owner |
|---|---|---|---|---|---|
| M0 — Project bootstrap and repo truth | NOT_STARTED | none | none | inspect workspace and create repo truth docs | Claude |
| M1 — First drivable core | NOT_STARTED | none | none | create input/vehicle baseline after M0 | Claude |
| M2 — Surface system and telemetry | NOT_STARTED | none | none | initialize surface data and telemetry after M1 | Claude |
| M3 — Vehicle trio differentiation | NOT_STARTED | none | none | create vehicle specs and spawn path after M2 | Claude |
| M4 — Mechanical damage consequences | NOT_STARTED | none | none | build damage component and data after M3 | Claude |
| M5 — Main world greybox | NOT_STARTED | none | none | build compact route structure after M4 | Claude |
| M6 — Activities, HUD, and save/load | NOT_STARTED | none | none | implement structured loop after M5 | Claude |
| M7 — Traffic, day/night, wet-world integration, activity completion | NOT_STARTED | none | none | integrate systemic world behavior after M6 | Claude |
| M8 — Hardening, performance, and release-candidate pass | NOT_STARTED | none | none | stabilize and document after M7 | Claude |

## Rules
- Only one milestone should usually be IN_PROGRESS at a time.
- A milestone can become VERIFIED only if the relevant gates are logged in docs/VERIFICATION.md.
- If a milestone is BLOCKED, write the blocker precisely and keep the repo improving through the fallback path.

```

## FILE: docs/TEST_MATRIX.md
```md
# TEST_MATRIX

## Status legend
- PASS
- FAIL
- PARTIAL
- NOT_RUN

| Test ID | Milestone | Vehicle | Surface | Scenario | Target / Intent | Observed | Status | Notes |
|---|---|---|---|---|---|---|---|---|
| T-001 | M1 | Hatch | Dry Asphalt | Spawn and drive smoke test | vehicle can accelerate, steer, brake, reset | pending | NOT_RUN | |
| T-002 | M2 | Hatch | Dry Asphalt | Surface baseline loop | establish dry reference feel and telemetry | pending | NOT_RUN | |
| T-003 | M2 | Hatch | Gravel | Surface comparison loop | obvious grip and braking difference vs dry | pending | NOT_RUN | |
| T-004 | M2 | Hatch | Wet Asphalt | Wet comparison loop | obvious slip increase vs dry | pending | NOT_RUN | |
| T-005 | M3 | Coupe | Dry Asphalt | Agility loop / skidpad | more rotation and responsiveness than hatch | pending | NOT_RUN | |
| T-006 | M3 | Hatch | Dry Asphalt | Stability loop | more forgiving than coupe | pending | NOT_RUN | |
| T-007 | M3 | Pickup | Dry Asphalt | Lane-change / bump lane | more roll and slower transitions | pending | NOT_RUN | |
| T-008 | M4 | Hatch | Dry Asphalt | Light frontal impact | minor drivability change | pending | NOT_RUN | |
| T-009 | M4 | Hatch | Dry Asphalt | Medium side impact | clear steering / stability penalty | pending | NOT_RUN | |
| T-010 | M4 | Pickup | Dry Asphalt | Heavy offset impact | severe top-speed / stability degradation | pending | NOT_RUN | |
| T-011 | M6 | Any | Any | Checkpoint sprint | success / fail / reset all function | pending | NOT_RUN | |
| T-012 | M6 | Any | Any | Save / load smoke | key state persists correctly | pending | NOT_RUN | |
| T-013 | M7 | Any | Wet Asphalt | In-world wet corridor | in-world wet handling difference is obvious | pending | NOT_RUN | |
| T-014 | M7 | Any | Any | Traffic corridor smoke | traffic follows path and recovers simply | pending | NOT_RUN | |

## Measurement notes
Use this section for quick measured values when available:
- acceleration segments
- braking distances
- skidpad or loop balance notes
- slalom / lane-change notes
- bump absorption notes
- post-impact drivability notes

```

## FILE: docs/RISK_REGISTER.md
```md
# RISK_REGISTER

| ID | Risk | Impact | Likelihood | Mitigation | Owner | Status |
|---|---|---|---|---|---|---|
| R-001 | UE project or engine tooling unavailable in current environment | High | Medium | create full text-side scaffolding, BUILD_RUNBOOK, EDITOR_BOOTSTRAP, and asset manifest | Claude | OPEN |
| R-002 | Editor-generated assets cannot be created directly | High | High | document exact asset creation steps, create helper scripts, keep code/data path moving | Claude | OPEN |
| R-003 | Chaos default handling feels template-like | High | Medium | enforce telemetry, tuning notes, and differentiated specs early | Claude | OPEN |
| R-004 | World scope balloons into empty map | High | Medium | keep compact route-first layout and milestone gates | Claude | OPEN |
| R-005 | Traffic AI steals time from drivability core | Medium | Medium | delay traffic complexity until after first structured loop | Claude | OPEN |
| R-006 | Damage model remains cosmetic | High | Medium | require mechanical consequences and verification notes | Claude | OPEN |
| R-007 | Docs drift away from repository reality | Medium | Medium | reconcile docs at every session start and after verification | Claude | OPEN |
| R-008 | Save/load becomes overbuilt too early | Medium | Low | keep SaveGame narrow in v1 | Claude | OPEN |

## Status values
- OPEN
- MONITORING
- MITIGATED
- CLOSED

```

## FILE: docs/STATE.md
```md
# STATE

## Snapshot
- Date:
- Current milestone:
- Current status:
- Current gate:
- Repository truth summary:

## Last verified action
- Command or action:
- Result:
- Evidence location:

## Current blockers / risks
- Blocker 1:
- Blocker 2:

## Files changed in last pass
- 

## Next immediate actions
1. 
2. 
3. 

## Notes for next session
- 

```

## FILE: docs/TODO.md
```md
# TODO

## Now
- [ ] inspect workspace reality
- [ ] create or reconcile milestone docs
- [ ] advance the next unchecked verification gate

## Next
- [ ] 

## Later
- [ ] 

## Deferred
- [ ] 

```

## FILE: docs/VERIFICATION.md
```md
# VERIFICATION

| Date/Time | Milestone | Command / Action | Result | Evidence | Next Action |
|---|---|---|---|---|---|
| pending | M0 | initial workspace inspection | pending | pending | create baseline docs and run first validation |

```

## FILE: docs/DECISIONS.md
```md
# DECISIONS

| Date | ID | Decision | Why | Consequence |
|---|---|---|---|---|
| pending | D-001 | Unreal Engine 5.x with C++-first architecture | matches project goals and technical contract | Blueprint use stays selective |
| pending | D-002 | Compact dense world over giant map | better replay value and faster milestone progress | world scope remains constrained |
| pending | D-003 | Data-driven tuning for vehicles, surfaces, damage, and activities | easier tuning, testing, and continuation | more up-front data asset work |

```

## FILE: docs/BUILD_RUNBOOK.md
```md
# BUILD_RUNBOOK

## Environment
- Host OS:
- Unreal Engine version:
- Engine install path:
- Repo root:
- Shell:

## Known commands
### Regenerate project files
- Command:
- Notes:

### Build editor / project
- Command:
- Notes:

### Open editor
- Command:
- Notes:

### Data validation
- Command:
- Notes:

### Smoke run
- Command:
- Notes:

### Packaging / cook
- Command:
- Notes:

## Common failure signatures
- 

## Fixes that worked
- 

```

## FILE: docs/EDITOR_BOOTSTRAP.md
```md
# EDITOR_BOOTSTRAP

Use this file when the current environment cannot create editor-generated assets directly.

## 1) Enable required plugins
- Enhanced Input
- Chaos Vehicles / Chaos Vehicle Plugin
- Data Validation
- any project-approved helper plugin only if already part of the plan

## 2) Create required maps
- L_MZ_Boot
- L_MZ_Frontend
- L_MZ_TestTrack
- L_MZ_MainWorld

## 3) Create required input assets
### Mapping contexts
- IMC_MZ_OnFoot
- IMC_MZ_Drive
- IMC_MZ_UI

### Input actions
- IA_MZ_Move
- IA_MZ_Look
- IA_MZ_Jump
- IA_MZ_Interact
- IA_MZ_Pause
- IA_MZ_Steer
- IA_MZ_Throttle
- IA_MZ_Brake
- IA_MZ_Handbrake
- IA_MZ_ShiftUp
- IA_MZ_ShiftDown
- IA_MZ_CameraCycle
- IA_MZ_ResetVehicle

## 4) Create required data assets
### Vehicle specs
- DA_MZVehicle_Coupe
- DA_MZVehicle_Hatch
- DA_MZVehicle_Pickup

### Surface profiles
- DA_MZSurface_DryAsphalt
- DA_MZSurface_WetAsphalt
- DA_MZSurface_Gravel
- DA_MZSurface_Grass

### Activity definitions
- DA_MZActivity_CheckpointSprint_DockLoop
- DA_MZActivity_Delivery_ServiceCorridor
- DA_MZActivity_Weave_RingRoad
- DA_MZActivity_DamageSurvival_TestLane

## 5) Create UI assets
- WBP_MZHUD
- WBP_MZPauseMenu
- WBP_MZMainMenu
- WBP_MZTelemetry

## 6) Build the first test content
- a flat or lightly varied L_MZ_TestTrack
- one hatchback setup connected to AMZVehiclePawn or the chosen vehicle BP wrapper
- a player spawn / possession path
- at least two surface zones: Dry Asphalt and Gravel
- one recovery / reset point

## 7) Main world bootstrap
Create or reserve:
- city district greybox
- industrial zone greybox
- highway / ring-road segment
- rural / gravel spur
- proving ground

Create Data Layers:
- DL_MZ_City
- DL_MZ_Industrial
- DL_MZ_Highway
- DL_MZ_Rural
- DL_MZ_ProvingGround
- DL_MZ_Activities
- DL_MZ_Debug

## 8) Hook verification back into docs
After manual editor steps:
- update docs/ASSET_MANIFEST.md
- update docs/STATE.md
- update docs/MILESTONE_STATUS.md
- add a verification entry in docs/VERIFICATION.md

```

## FILE: docs/ASSET_MANIFEST.md
```md
# ASSET_MANIFEST

| Asset Name | Type | Folder | Purpose | Status | Replacement Notes |
|---|---|---|---|---|---|
| L_MZ_Boot | Map | Content/MileZero/Maps/Boot | boot / startup flow | PLANNED | |
| L_MZ_Frontend | Map | Content/MileZero/Maps/Frontend | menu / frontend | PLANNED | |
| L_MZ_TestTrack | Map | Content/MileZero/Maps/Test | first drivable verification map | PLANNED | |
| L_MZ_MainWorld | Map | Content/MileZero/Maps/World | main compact open world | PLANNED | |
| IMC_MZ_OnFoot | Input Mapping Context | Content/MileZero/Input | on-foot mapping | PLANNED | |
| IMC_MZ_Drive | Input Mapping Context | Content/MileZero/Input | driving mapping | PLANNED | |
| IMC_MZ_UI | Input Mapping Context | Content/MileZero/Input | menu / UI mapping | PLANNED | |
| DA_MZVehicle_Coupe | Primary Data Asset | Content/MileZero/Data/Vehicles | coupe tuning spec | PLANNED | |
| DA_MZVehicle_Hatch | Primary Data Asset | Content/MileZero/Data/Vehicles | hatch tuning spec | PLANNED | |
| DA_MZVehicle_Pickup | Primary Data Asset | Content/MileZero/Data/Vehicles | pickup tuning spec | PLANNED | |
| DA_MZSurface_DryAsphalt | Primary Data Asset | Content/MileZero/Data/Surfaces | dry grip profile | PLANNED | |
| DA_MZSurface_WetAsphalt | Primary Data Asset | Content/MileZero/Data/Surfaces | wet grip profile | PLANNED | |
| DA_MZSurface_Gravel | Primary Data Asset | Content/MileZero/Data/Surfaces | gravel grip profile | PLANNED | |
| DA_MZSurface_Grass | Primary Data Asset | Content/MileZero/Data/Surfaces | grass grip profile | PLANNED | |
| WBP_MZHUD | Widget Blueprint | Content/MileZero/UI | runtime HUD | PLANNED | |
| WBP_MZPauseMenu | Widget Blueprint | Content/MileZero/UI | pause flow | PLANNED | |
| WBP_MZMainMenu | Widget Blueprint | Content/MileZero/UI | frontend menu | PLANNED | |
| WBP_MZTelemetry | Widget Blueprint | Content/MileZero/UI | telemetry overlay | PLANNED | |

## Status values
- PLANNED
- CREATED
- WIRED
- VERIFIED
- REPLACED

```
