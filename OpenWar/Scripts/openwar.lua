openwar = {}

openwar.Unit = {}

openwar.units = {}


function openwar.Unit:new(player, platform, weapon, strength, x, y, facing)
    local o =
    {
        unit_id = battle_new_unit(player, platform, weapon, strength, x, y, facing),
        player = player,
        platform = platform,
        weapon = weapon,
        strength = strength,
        status = 0,
        x = x,
        y = y,
        facing = facing,
        range = 150
    }
    setmetatable(o, self)
    self.__index = self
    openwar.units[o.unit_id] = o;
    return o
end


function openwar.Unit:movement(running, path, charge_unit, heading)
    local charge_unit_id
    if charge_unit ~= nil then
        charge_unit_id = charge_unit.unit_id
    end
    battle_set_unit_movement(self.unit_id, running, path, charge_unit_id, heading)
end


function openwar.Unit:refresh()
    local x, y, heading, status, strength, morale = battle_get_unit_status(self.unit_id)
    self.x = x
    self.y = y
    self.strength = strength
    self.status = status
end


return openwar
