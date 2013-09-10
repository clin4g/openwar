-- Copyright (C) 2013 Felix Ungman
--
-- This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

openwar_terrain_init("smooth", openwar_script_directory .. "/DefaultMap.tiff", 1024)
openwar_simulator_init()


battle_message("Hello World")


battle_new_unit(1, "GEN", "KATA",  40, 479, 318, 0)
battle_new_unit(1, "ASH", "YARI", 80, 481, 364, 0)
battle_new_unit(1, "SAM", "BOW",  80, 360, 399, 0)
battle_new_unit(1, "SAM", "BOW",  80, 422, 397, 0)
battle_new_unit(1, "SAM", "BOW",  80, 548, 398, 0)
battle_new_unit(1, "SAM", "BOW",  80, 615, 401, 0)
battle_new_unit(1, "SAM", "ARQ",  80, 320, 440, 0)
battle_new_unit(1, "SAM", "ARQ",  80, 643, 444, 0)

battle_new_unit(2, "SAM", "YARI", 80, 440, 610, 180)
battle_new_unit(2, "SAM", "YARI", 80, 539, 610, 180)
battle_new_unit(2, "SAM", "KATA", 80, 405, 641, 180)
battle_new_unit(2, "SAM", "KATA", 80, 576, 636, 180)
battle_new_unit(2, "CAV", "BOW",  40, 308, 679, 180)
battle_new_unit(2, "SAM", "KATA", 80, 488, 637, 180)
battle_new_unit(2, "SAM", "NAGI", 80, 447, 665, 180)
battle_new_unit(2, "SAM", "NAGI", 80, 529, 672, 180)
battle_new_unit(2, "CAV", "KATA", 40, 680, 673, 180)
