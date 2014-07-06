function CastSpell(self,spellIndex,x,y)
        if spellIndex == 0 then 
                spell = CSpellWork(spellIndex,'VayneTumble',x,y) --EzrealMysticShot
                spell.updatePos = true
                spell.stop = true
                --spell.setFacing = true

                spell:AddMissile(self,'EzrealMysticShotMissile',0.25)
                spell:AddParticle(self,'Ezreal_bow.troy',0.25)
                
                self:QueueSpell(spell)
                --[[self:SetupSpell(1)
                self:CastSpell(spellIndex,x,y)

                --self:SetPosition(...) --4F sets face position
                CreateMissile(self,'EzrealMysticShotMissile',x,y) --3A CreateMissile
                
                pe = ParticleEmiter()
                pe:AddParticle(self,'Ezreal_bow.troy')
                pe:Send(self)]]
        elseif spellIndex == 1 then 
                self:SetupSpell(1)
                self:CastSpell(spellIndex,x,y)

                --self:SetPosition(...) --4F sets face position
                --self:CastSpell(spellIndex,x,y) ??? Different Hash --EzrealEssenceFluxMissile?
                
                pe = ParticleEmiter()
                pe:AddParticle(self,'Ezreal_bow_yellow.troy')
                pe:Send(self)
        elseif spellIndex == 2 then 
                self:SetupSpell(1)
                self:CastSpell(spellIndex,x,y)

                --self:SetPosition(...) --4F sets face position
                
                pe = ParticleEmiter()
                pe:AddParticle(self,'Ezreal_arcaneshift_cas.troy') --Applied to (null) < NOT OK - APPLY TO NULL
                pe:AddParticle(self,'Ezreal_arcaneshift_flash.troy') --Applied to why so mad
                pe:Send(self)
        elseif spellIndex == 3 then 
                self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4
        end
end