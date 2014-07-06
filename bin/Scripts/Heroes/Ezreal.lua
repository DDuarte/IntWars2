function CastSpell(self,spellIndex,x,y)
        if spellIndex == 0 then 
                spell = CSpellWork(self,spellIndex,x,y) --EzrealMysticShot
                spell.iUpdatePos = 1
                --spell.setFacing = true --4F
                spell.cooldown = 0
                spell.manaCost = 0

                spell:AddMissile(self,'EzrealMysticShotMissile',0.25)
                spell:AddParticle(self,self,'Ezreal_bow.troy',0.25)
                
                self:QueueSpell(spell)
                --[[self:SetupSpell(1)
                self:CastSpell(spellIndex,x,y)

                --self:SetPosition(...) --4F sets face position
                CreateMissile(self,'EzrealMysticShotMissile',x,y) --3A CreateMissile
                
                pe = ParticleEmiter()
                pe:AddParticle(self,'Ezreal_bow.troy')
                pe:Send(self)]]
        elseif spellIndex == 1 then 
                --[[--self:SetupSpell(1)
                self:CastSpell(spellIndex,x,y)

                --self:SetPosition(...) --4F sets face position
                --self:CastSpell(spellIndex,x,y) ??? Different Hash --EzrealEssenceFluxMissile?
                
                pe = ParticleEmiter()
                pe:AddParticle(self,'Ezreal_bow_yellow.troy')
                pe:Send(self)]]
                spell = CSpellWork(self,spellIndex,x,y) --EzrealMysticShot
                spell.iUpdatePos = 1
                spell.stop = true
                --spell.setFacing = true --4F
                spell.cooldown = 0
                spell.manaCost = 0

                spell:AddSpell(0x43,'EzrealEssenceFluxMissile',0.25) --release missle instant with 6D
                spell:AddParticle(self,self,'Ezreal_bow_yellow.troy',0.25)
                
                self:QueueSpell(spell)
        elseif spellIndex == 2 then 
                --[[self:SetupSpell(1)
                self:CastSpell(spellIndex,x,y)

                --self:SetPosition(...) --4F sets face position
                
                pe = ParticleEmiter()
                pe:AddParticle(self,'Ezreal_arcaneshift_cas.troy') --Applied to (null) < NOT OK - APPLY TO NULL
                pe:AddParticle(self,'Ezreal_arcaneshift_flash.troy') --Applied to why so mad
                pe:Send(self)]]
                spell = CSpellWork(self,spellIndex,x,y) --EzrealMysticShot
                spell.iUpdatePos = 1
                --spell.setFacing = true --4F --after 250
                spell.cooldown = 0
                spell.manaCost = 0

                spell:AddParticle(self,self,'Ezreal_arcaneshift_cas.troy',0.25)
                spell:AddParticle(self,self,'Ezreal_arcaneshift_flash.troy',0.25)
                spell:SetTeleport(x,y,0.25)
                
                self:QueueSpell(spell)
        elseif spellIndex == 3 then 
                --[[spell = CSpellWork(self,spellIndex,x,y)
                spell.fDelay
                
                self:QueueSpell(spell)]]
                self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4
        end
end