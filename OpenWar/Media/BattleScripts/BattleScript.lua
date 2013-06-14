-- Copyright (C) 2013 Felix Ungman
--
-- This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

-- unit states:
-- 0 = STANDING
-- 1 = WALKING
-- 2 = RUNNING
-- 3 = ROUTING
-- 4 = CHARGING
-- 5 = FIGHTING
-- 6 = SHOOTING

-- function battle_new_unit(...)
-- unit_id = battle_new_unit(player, platform, weapon, strength, x, y, heading)
--     player (1 = human, 2 = AI)
--     platform ("GEN", "CAV", "SAM", "ASH")
--     weapon ("YARI", "KATA", "NAGI", "BOW", "ARQ")
--     strength (the number of soldiers)
--     x, y (map coordinates)
--     heading (degrees from north)

-- battle_set_unit_movement(unit_id, running, path, charge_id, heading)
--     unit_id
--     running (true or false)
--     path  {{x=<x1>, y=<y1>}, {x=<x2>,y=<y2}, ...}
--     charge_id
--     heading (degrees from north)

-- x, y, heading, state, strength, morale = battle_get_unit_status(unit_id)
    --return 232, 313, 34, 0, 23, 45, 0.5


-- function set_unit_missile_target(unit_id, target_id)


openwar_terrain_init("smooth")
openwar_simulator_init()


battle_message("Hello World")


battle_get_time()

battle_new_unit(1, "SAM", "KATA", 80, 400, 400, 0)
battle_new_unit(1, "ASH", "YARI", 80, 400, 450, 0)
battle_new_unit(1, "ASH", "YARI", 80, 400, 500, 0)

unit1 = battle_new_unit(2, "SAM", "KATA", 80, 600, 400, 0)
unit2 = battle_new_unit(2, "ASH", "YARI", 80, 600, 450, 0)
unit3 = battle_new_unit(2, "ASH", "YARI", 80, 600, 500, 0)

battle_message("xxx")

battle_set_unit_movement(unit3, false, {{x=550, y=550}, {x=600, y=600}}, nil, 0)

x, y, heading, state, strength, morale = battle_get_unit_status(unit3)

battle_message("status" .. x)
