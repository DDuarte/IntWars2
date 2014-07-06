--[[
NotSingleTargetSpell = true
DoesntBreakShields = true
DoesntTriggerSpellCasts = false
CastingBreaksStealth = true
IsDamagingSpell = true

local R1 = {Function = BBPreloadParticle}
R1["Params"] = {Name = 'laywaste_point.troy'} --R2 = {} / R2["Name"] = "laywaste_point.troy" / R1["Params"] = R2

local R2 = {Function = BBPreloadParticle}
R2["Params"] = {Name = 'laywaste_tar.troy'} --R3 = {} / R3["Name"] = "laywaste_point.troy" / R2["Params"] = R3

local R3 = {Function = BBPreloadCharacter}
R3["Params"] = {Name = 'testcube'} --R4 = {} / R4["Name"] = "testcube" / R3["Params"] = R4

local R4 = {Function = BBPreloadCharacter}
R4["Params"] = {Name = 'spellbook1'} --R5 = {} / R5["Name"] = "testcube" / R4["Params"] = R5

local R0 = {R1,R2,R3,R4}
PreLoadBuildingBlocks = R0
]]

--[[   35 [-]: SETLIST   R0 4 1       ; R0[(1-1)*FPF+i] := R(0+i), 1 <= i <= 4
   36 [-]: SETGLOBAL R0 K5        ; PreLoadBuildingBlocks := R0
   37 [-]: RETURN    R0 1         ; return ]]
--PreLoadBuildingBlocks = R1,R2,R3,R4;


function CastSpell(self,spellIndex,x,y)
        if spellIndex == 0 then 
                spell = CSpellWork(self,spellIndex,x,y) --LayWaste
                spell.updatePos = true
                spell.stop = true
                
                --local p1 = spell:AddParticle(self:GetParticle('LayWaste_point.troy'),spell.castTime) --Wind up 0.25
                p1 = spell:AddParticle(self,'LayWaste_point.troy',0.25)
                --p1.vision = true
                
                --spell:AddParticle(self:GetParticle('LayWaste_tar.troy'),spell.castFrame/30) --burn time 0.6
                p2 = spell:AddParticle(self,'LayWaste_tar.troy',0.6)
                
                self:QueueSpell(spell)
                --[[
                local spell = CSpell('LayWaste',x,y) --LayWaste
                spell.updatePos = true
                spell.stop = true
                
                --local p1 = spell:AddParticle(self:GetParticle('LayWaste_point.troy'),spell.castTime) --Wind up 0.25
                local p1 = spell:AddParticle(self,'LayWaste_point.troy',0.25)
                p1.vision = true
                
                --spell:AddParticle(self:GetParticle('LayWaste_tar.troy'),spell.castFrame/30) --burn time 0.6
                local p2 = spell:AddParticle(self,'LayWaste_tar.troy',0.6)
                
                self:QueueSpell(spell)
                ]]

                --self:SetupSpell() --send FE 0F
                --self:CastSpell(spellIndex,x,y)

                --MoveAns(self,...) --60
                --Vision Spawn for the Q particle
                --pe = ParticleEmiter()
                --pe:AddParticle(self,nil,'LayWaste_point.troy')
                --pe:Send(self)
                --after 1 second
                --KillParticle(self,...) --[40000063] 37
                --pe:AddParticle(self,'LayWaste_tar.troy') --Applied to (null)
        elseif spellIndex == 1 then 
                self:SetupSpell(15,1,1)
                self:CastSpell(spellIndex,x,y)

                --MoveAns(self,...) --60
                
                pe = ParticleEmiter()
                pe:AddParticle(self,nil,'wallofpain__new_beam.troy') --applied to hiu???
                --pe:AddParticle(self,'wallofpain_new_post_green.troy') --Applied to (null)
                --pe:AddParticle(self,'wallofpain_new_post_green.troy') --Applied to (null)
                pe:Send(self)
        elseif spellIndex == 2 then
                --[[self:CastSpell(spellIndex,x,y)

                --self:SetPosition(...) --4F sets face position
                
                pe = ParticleEmiter()
                pe:AddParticle(self,'Defile_green_cas.troy') 
                pe:Send(self)

                self:ApplyBuff(2,'Defile',30000.000000) --Stacks: 1, Visible: 0]]
                spell = CSpellWork(self,spellIndex,x,y) --LayWaste
                spell.updatePos = true
                spell.stop = true
                
                p1 = spell:AddParticle(self,self,'Defile_green_cas.troy',0.25)
                spell:AddBuff(2,'Defile',30000.000000,1) --Stacks: 1, Visible: 0]]
                
                self:QueueSpell(spell)
        elseif spellIndex == 3 then 
                self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4
        end
end