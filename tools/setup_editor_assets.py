"""
MileZero — First-boot editor asset creation script.
Run inside UE5 Editor via: Edit > Editor Preferences > Python > Execute File
Or via command line: UnrealEditor-Cmd.exe MileZero.uproject -run=pythonscript -script=tools/setup_editor_assets.py

Creates the minimum required assets for M1 first drivable loop:
- Enhanced Input actions and mapping context
- Vehicle data asset (hatch)
- Surface data assets (dry asphalt, gravel)
- Test track map placeholder

Requires: ChaosVehiclesPlugin, EnhancedInput enabled in .uproject (already configured).
"""

import unreal

# ─── Helpers ─────────────────────────────────────────────────────

def get_or_create_dir(path):
    """Ensure a content directory exists."""
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)
    return path

def save_asset(asset):
    """Save a newly created asset to disk."""
    unreal.EditorAssetLibrary.save_loaded_asset(asset)

# ─── Enhanced Input Actions ──────────────────────────────────────

INPUT_DIR = "/Game/MileZero/Input"

def create_input_action(name, value_type):
    """Create an InputAction asset with the given value type."""
    path = f"{INPUT_DIR}/{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        unreal.log(f"  Already exists: {path}")
        return unreal.EditorAssetLibrary.load_asset(path)

    factory = unreal.InputActionFactory()
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    action = asset_tools.create_asset(name, INPUT_DIR, unreal.InputAction, factory)

    if action is not None:
        action.set_editor_property("value_type", value_type)
        save_asset(action)
        unreal.log(f"  Created: {path}")
    return action

def create_input_assets():
    """Create all required input actions and the driving mapping context."""
    get_or_create_dir(INPUT_DIR)
    unreal.log("Creating Enhanced Input actions...")

    # Input actions with their value types
    actions = {
        "IA_MZ_Throttle":     unreal.InputActionValueType.AXIS1D,  # float 0-1
        "IA_MZ_Brake":        unreal.InputActionValueType.AXIS1D,
        "IA_MZ_Steer":        unreal.InputActionValueType.AXIS1D,  # float -1 to 1
        "IA_MZ_Handbrake":    unreal.InputActionValueType.BOOLEAN,
        "IA_MZ_ShiftUp":      unreal.InputActionValueType.BOOLEAN,
        "IA_MZ_ShiftDown":    unreal.InputActionValueType.BOOLEAN,
        "IA_MZ_CameraCycle":  unreal.InputActionValueType.BOOLEAN,
        "IA_MZ_ResetVehicle": unreal.InputActionValueType.BOOLEAN,
        "IA_MZ_Look":         unreal.InputActionValueType.AXIS2D,
        "IA_MZ_Move":         unreal.InputActionValueType.AXIS2D,
        "IA_MZ_Jump":         unreal.InputActionValueType.BOOLEAN,
        "IA_MZ_Interact":     unreal.InputActionValueType.BOOLEAN,
        "IA_MZ_Pause":        unreal.InputActionValueType.BOOLEAN,
    }

    created = {}
    for name, vtype in actions.items():
        created[name] = create_input_action(name, vtype)

    # Create IMC_MZ_Drive mapping context
    imc_path = f"{INPUT_DIR}/IMC_MZ_Drive"
    if not unreal.EditorAssetLibrary.does_asset_exist(imc_path):
        factory = unreal.InputMappingContextFactory()
        asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
        imc = asset_tools.create_asset("IMC_MZ_Drive", INPUT_DIR, unreal.InputMappingContext, factory)

        if imc is not None:
            # Add key mappings for driving
            # W = Throttle
            if created.get("IA_MZ_Throttle"):
                mapping = imc.map_key(created["IA_MZ_Throttle"], unreal.Key(key_name="W"))

            # S = Brake
            if created.get("IA_MZ_Brake"):
                mapping = imc.map_key(created["IA_MZ_Brake"], unreal.Key(key_name="S"))

            # A/D = Steer (A = -1, D = +1)
            if created.get("IA_MZ_Steer"):
                mapping_d = imc.map_key(created["IA_MZ_Steer"], unreal.Key(key_name="D"))
                mapping_a = imc.map_key(created["IA_MZ_Steer"], unreal.Key(key_name="A"))
                # A needs a negate modifier
                negate = unreal.InputModifierNegate()
                mapping_a.add_modifier(negate)

            # Space = Handbrake
            if created.get("IA_MZ_Handbrake"):
                imc.map_key(created["IA_MZ_Handbrake"], unreal.Key(key_name="SpaceBar"))

            # C = Camera cycle
            if created.get("IA_MZ_CameraCycle"):
                imc.map_key(created["IA_MZ_CameraCycle"], unreal.Key(key_name="C"))

            # R = Reset vehicle
            if created.get("IA_MZ_ResetVehicle"):
                imc.map_key(created["IA_MZ_ResetVehicle"], unreal.Key(key_name="R"))

            # Mouse XY = Look
            if created.get("IA_MZ_Look"):
                imc.map_key(created["IA_MZ_Look"], unreal.Key(key_name="Mouse2D"))

            # Gamepad right trigger = Throttle
            if created.get("IA_MZ_Throttle"):
                imc.map_key(created["IA_MZ_Throttle"], unreal.Key(key_name="Gamepad_RightTriggerAxis"))

            # Gamepad left trigger = Brake
            if created.get("IA_MZ_Brake"):
                imc.map_key(created["IA_MZ_Brake"], unreal.Key(key_name="Gamepad_LeftTriggerAxis"))

            # Gamepad left stick X = Steer
            if created.get("IA_MZ_Steer"):
                imc.map_key(created["IA_MZ_Steer"], unreal.Key(key_name="Gamepad_LeftX"))

            # Gamepad A = Handbrake
            if created.get("IA_MZ_Handbrake"):
                imc.map_key(created["IA_MZ_Handbrake"], unreal.Key(key_name="Gamepad_FaceButton_Bottom"))

            save_asset(imc)
            unreal.log(f"  Created: {imc_path}")
    else:
        unreal.log(f"  Already exists: {imc_path}")

    return created

# ─── Vehicle Data Asset ──────────────────────────────────────────

def create_vehicle_data_assets():
    """Create the hatch vehicle data asset with FWD hatchback defaults."""
    veh_dir = "/Game/MileZero/Data/Vehicles"
    get_or_create_dir(veh_dir)
    unreal.log("Creating vehicle data assets...")

    path = f"{veh_dir}/DA_MZVehicle_Hatch"
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        unreal.log(f"  Already exists: {path}")
        return

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    # UMZVehicleDataAsset is a C++ class — create an instance
    da = asset_tools.create_asset(
        "DA_MZVehicle_Hatch", veh_dir,
        unreal.load_class(None, "/Script/MileZero.MZVehicleDataAsset"),
        None  # no factory needed for data assets
    )

    if da is not None:
        da.set_editor_property("VehicleID", "Hatch")
        da.set_editor_property("DisplayName", unreal.Text("Practical Hatchback"))
        da.set_editor_property("MassKg", 1180.0)
        da.set_editor_property("CenterOfMassOffset", unreal.Vector(0.0, 0.0, -10.0))
        da.set_editor_property("DragCoefficient", 0.32)
        da.set_editor_property("DrivetrainType", 1)  # FWD enum value
        da.set_editor_property("GearRatios", [3.6, 2.1, 1.4, 1.0, 0.77])
        da.set_editor_property("FinalDriveRatio", 3.9)
        da.set_editor_property("MaxSteerAngle", 38.0)
        # Engine sub-struct
        engine = da.get_editor_property("Engine")
        engine.set_editor_property("MaxTorque", 280.0)
        engine.set_editor_property("MaxRPM", 6800.0)
        engine.set_editor_property("IdleRPM", 900.0)
        engine.set_editor_property("EngineRevDownRate", 2000.0)
        # Brakes sub-struct
        brakes = da.get_editor_property("Brakes")
        brakes.set_editor_property("MaxBrakeTorque", 2800.0)
        brakes.set_editor_property("HandbrakeTorque", 4000.0)
        brakes.set_editor_property("FrontBrakeBias", 0.65)
        save_asset(da)
        unreal.log(f"  Created: {path}")

# ─── Surface Data Assets ─────────────────────────────────────────

def create_surface_data_assets():
    """Create dry asphalt and gravel surface profiles."""
    surf_dir = "/Game/MileZero/Data/Surfaces"
    get_or_create_dir(surf_dir)
    unreal.log("Creating surface data assets...")

    surfaces = [
        ("DA_MZSurface_DryAsphalt", "DryAsphalt", "Dry Asphalt", 1.0, 1.0, 0.0),
        ("DA_MZSurface_Gravel",     "Gravel",     "Gravel",      0.62, 1.8, 0.35),
    ]

    for name, sid, display, grip, resistance, roughness in surfaces:
        path = f"{surf_dir}/{name}"
        if unreal.EditorAssetLibrary.does_asset_exist(path):
            unreal.log(f"  Already exists: {path}")
            continue

        asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
        da = asset_tools.create_asset(
            name, surf_dir,
            unreal.load_class(None, "/Script/MileZero.MZSurfaceDataAsset"),
            None
        )
        if da is not None:
            da.set_editor_property("SurfaceID", sid)
            da.set_editor_property("DisplayName", unreal.Text(display))
            da.set_editor_property("GripMultiplier", grip)
            da.set_editor_property("RollingResistance", resistance)
            da.set_editor_property("Roughness", roughness)
            save_asset(da)
            unreal.log(f"  Created: {path}")

# ─── Test Map ─────────────────────────────────────────────────────

def create_test_map():
    """Create L_MZ_TestTrack with a flat ground plane and spawn point."""
    map_dir = "/Game/MileZero/Maps/Test"
    map_path = f"{map_dir}/L_MZ_TestTrack"
    get_or_create_dir(map_dir)

    if unreal.EditorAssetLibrary.does_asset_exist(map_path):
        unreal.log(f"  Test map already exists: {map_path}")
        return

    unreal.log("Creating L_MZ_TestTrack...")

    # Create a new level
    editor_level = unreal.EditorLevelLibrary
    success = unreal.EditorLevelLibrary.new_level(map_path)

    if success:
        world = unreal.EditorLevelLibrary.get_editor_world()

        # Add a large floor plane for driving
        mesh_path = "/Engine/BasicShapes/Plane.Plane"
        floor_loc = unreal.Vector(0.0, 0.0, 0.0)
        floor_rot = unreal.Rotator(0.0, 0.0, 0.0)
        floor = unreal.EditorLevelLibrary.spawn_actor_from_class(
            unreal.StaticMeshActor, floor_loc, floor_rot
        )
        if floor:
            mesh_comp = floor.get_component_by_class(unreal.StaticMeshComponent)
            if mesh_comp:
                plane_mesh = unreal.load_asset(mesh_path)
                mesh_comp.set_static_mesh(plane_mesh)
                # Scale to 200x200 meters (plane is 100x100 UU default, scale by 200)
                floor.set_actor_scale3d(unreal.Vector(200.0, 200.0, 1.0))
                floor.set_actor_label("Floor_DryAsphalt")

        # Add a player start
        ps = unreal.EditorLevelLibrary.spawn_actor_from_class(
            unreal.PlayerStart,
            unreal.Vector(0.0, 0.0, 100.0),
            unreal.Rotator(0.0, 0.0, 0.0)
        )
        if ps:
            ps.set_actor_label("MZ_VehicleSpawn")

        # Save the map
        unreal.EditorLevelLibrary.save_current_level()
        unreal.log(f"  Created: {map_path}")
    else:
        unreal.log_warning("  Failed to create test map")

# ─── Main ─────────────────────────────────────────────────────────

def main():
    unreal.log("=" * 60)
    unreal.log("MileZero First-Boot Asset Setup")
    unreal.log("=" * 60)

    create_input_assets()
    create_vehicle_data_assets()
    create_surface_data_assets()
    create_test_map()

    unreal.log("=" * 60)
    unreal.log("Setup complete. Next steps:")
    unreal.log("  1. Open L_MZ_TestTrack")
    unreal.log("  2. Set World Settings > GameMode Override to MZGameMode")
    unreal.log("  3. Assign IMC_MZ_Drive to MZPlayerController defaults")
    unreal.log("  4. Assign IA_MZ_* actions to MZVehiclePawn defaults or BP wrapper")
    unreal.log("  5. Assign DA_MZVehicle_Hatch to the vehicle pawn")
    unreal.log("  6. PIE to test first drivable loop")
    unreal.log("=" * 60)

if __name__ == "__main__":
    main()
