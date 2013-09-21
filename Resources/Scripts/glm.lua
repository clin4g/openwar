local M = {}

M.radians = math.rad
M.degrees = math.deg

M.sin = math.sin
M.cos = math.cos
M.tan = math.tan
M.asin = math.asin
M.acos = math.acos
M.atan = math.atan
M.atan2 = math.atan2

M.sqrt = math.sqrt

M.abs = math.abs
M.ceil = math.ceil
M.floor = math.floor


M.vec2 = {}

function M.vec2:new(x, y)
    o = { x = x, y = y }
    setmetatable(o, self)
    self.__index = self
    return o
end

function M.vec2.__tostring(v)
    return string.format("(%g, %g)", v.x, v.y)
end

function M.vec2.__eq(v1, v2)
    return v1.x == v2.x and v1.y == v2.y
end

function M.vec2.__lt(v1, v2)
    return v1.x < v2.x and v1.y < v2.y
end

function M.vec2.__le(v1, v2)
    return v1.x <= v2.x and v1.y <= v2.y
end

function M.vec2.__add(v1, v2)
    return M.vec2:new(v1.x + v2.x, v1.y + v2.y)
end

function M.vec2.__sub(v1, v2)
    return M.vec2:new(v1.x - v2.x, v1.y - v2.y)
end

function M.vec2.__mul(v, k)
    return M.vec2:new(v.x * k, v.y * k)
end

function M.vec2.__div(v, k)
    return M.vec2:new(v.x / k, v.y / k)
end

function M.length(v)
    local x = v.x
    local y = v.y
    return math.sqrt(x * x + y * y)
end

function M.distance(v1, v2)
    local x = v2.x - v1.x
    local y = v2.y - v1.y
    return math.sqrt(x * x + y * y)
end

function M.dot(v1, v2)
    return v1.x * v2.x + v1.y * v2.y
end

function M.normalize(v)
    return v / M.length(v)
end

return M
