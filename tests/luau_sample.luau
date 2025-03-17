--!strict
-- Enable strict type checking

type Fruit = {
    name: string,
    color: Color3,
    weight: number,
}

local function createFruit(name: string, color: Color3, weight: number): Fruit
    return {
        name = name,
        color = color,
        weight = weight
    }
end

-- Luau class example
local FruitBasket = {}
FruitBasket.__index = FruitBasket

function FruitBasket.new()
    local self = setmetatable({}, FruitBasket)
    self.fruits = {} :: {Fruit}
    return self
end

function FruitBasket:add(fruit: Fruit)
    table.insert(self.fruits, fruit)
end

-- Roblox API usage example
local basket = FruitBasket.new()
basket:add(createFruit("Apple", Color3.new(1, 0, 0), 0.2))
basket:add(createFruit("Banana", Color3.fromRGB(255, 255, 0), 0.15))

local part = Instance.new("Part")
part.Position = Vector3.new(0, 5, 0)
part.Parent = workspace

print("Fruit count:", #basket.fruits)

-- Roblox Vector3 example
local part = Instance.new("Part")
part.Position = Vector3.new(0, 5, 0)
part.Parent = workspace

-- Vector operations
local position = Vector3.new(3, 4, 0)
local velocity = Vector3.new(1, 2, 3)
local gravity = Vector3.new(0, -9.81, 0)

-- Basic vector math
local movement = velocity * 2
local new_position = position + movement + gravity

-- Dot product example
local direction = Vector3.new(1, 0, 0)
local speed = 5
local velocity_vector = direction * speed

-- Cross product example
local axis1 = Vector3.new(1, 0, 0)
local axis2 = Vector3.new(0, 1, 0)
local normal = axis1:Cross(axis2)

print("New position:", new_position)  --> (5, 4.38, 6)
print("Normal vector:", normal)       --> (0, 0, 1)
