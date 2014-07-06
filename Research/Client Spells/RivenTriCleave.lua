NotSingleTargetSpell = true
DoesntTriggerSpellCasts = false
IsDamagingSpell = true

BuffTextureName = "RivenBrokenWings.dds"
BuffName = "RivenTriCleaveBuff"

SpellToggleSlot = 1
SpellDamageRatio = 0.5

local R0 = {}

local R1 = {Function = BBPreloadSpell, Params = {Name = 'riventricleave'}}
local R2 = {Function = BBPreloadSpell, Params = {Name = 'riventricleavecooldown'}}
local R3 = {Function = BBPreloadSpell, Params = {Name = 'riventricleavesoundone'}}
local R4 = {Function = BBPreloadSpell, Params = {Name = 'riventricleavebuffer'}}
local R5 = {Function = BBPreloadSpell, Params = {Name = 'riventricleavedamage'}}
local R6 = {Function = BBPreloadSpell, Params = {Name = 'unlockanimation'}}
local R7 = {Function = BBPreloadSpell, Params = {Name = 'riventricleavesoundtwo'}}
local R8 = {Function = BBPreloadSpell, Params = {Name = 'riventricleavebufferb'}}
local R9 = {Function = BBPreloadSpell, Params = {Name = 'riventricleavesoundthree'}}
local R10 = {Function = BBPreloadSpell, Params = {Name = 'rivensword'}}

PreLoadBuildingBlocks = {R1,R2,R3,R4,R5,R6,R7,R8,R9,R10}