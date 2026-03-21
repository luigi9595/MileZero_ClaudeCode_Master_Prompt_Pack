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
