glm = require("glm")


local function assertFalse(value)
    if value then
        print("assert failed")
        print(debug.traceback())
    end
end

local function assertTrue(value)
    if not value then
        print("assert failed ")
        print(debug.traceback())
    end
end

local function assertEqual(expected, actual)
    if expected ~= actual then
        print("assert failed, expected " .. tostring(expected) .. ", actual " .. tostring(actual))
        print(debug.traceback())
    end
end


-- construction

assertEqual(1, glm.vec2:new(1, 2).x)
assertEqual(2, glm.vec2:new(1, 2).y)

-- tostring

assertEqual("(1, 2)", tostring(glm.vec2:new(1, 2)))

-- equality

assertTrue(glm.vec2:new(1, 2) == glm.vec2:new(1, 2))
assertFalse(glm.vec2:new(2, 1) == glm.vec2:new(1, 2))
assertTrue(glm.vec2:new(2, 1) ~= glm.vec2:new(1, 2))
assertFalse(glm.vec2:new(1, 2) ~= glm.vec2:new(1, 2))

-- addition

assertEqual(glm.vec2:new(6, 7), glm.vec2:new(5, 4) + glm.vec2:new(1, 3))

-- subtraction

assertEqual(glm.vec2:new(4, 1), glm.vec2:new(5, 4) - glm.vec2:new(1, 3))

-- multiplication

assertEqual(glm.vec2:new(4, 6), glm.vec2:new(2, 3) * 2)

-- division

assertEqual(glm.vec2:new(1, 1.5), glm.vec2:new(2, 3) /2)

-- length

assertEqual(5, glm.length(glm.vec2:new(3, 4)))

-- distance

assertEqual(5, glm.distance(glm.vec2:new(3, 4), glm.vec2:new(6, 8)))

-- dot

assertEqual(11, glm.dot(glm.vec2:new(1, 2), glm.vec2:new(3, 4)))

-- normalize

assertEqual(glm.vec2:new(1 / math.sqrt(2), 1 / math.sqrt(2)), glm.normalize(glm.vec2:new(2, 2)))



print(tostring(package.path))
