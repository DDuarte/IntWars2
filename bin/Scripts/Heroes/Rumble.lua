function CastSpell(self,spellIndex,x,y)
        if spellIndex == 0 then 
                spell = CSpellWork(spellIndex,'RumbleFlameThrower',x,y) --RumbleFlameThrower
                --spell.setFacing = true
                self:QueueSpell(spell)
                self:ApplyBuff(2,1,'RumbleFlameThrower',3.000000) --[[ TODO: create object to spawn gun particles {
                        pe:AddParticle(self,'rumble_gun_lite.troy') --Applied to why so mad
                        pe:AddParticle(self,'rumble_gun_cas.troy') --Applied to why so mad
                }]]
        elseif spellIndex == 1 then 
                spell = CSpellWork(spellIndex,'RumbleShield',x,y) --RenektonPreExecute

                spell.animation = 'Spell3'
                self:QueueSpell(spell)
                self:ApplyBuff(2,1,'RumbleShield',2.000000) --[[TODO: Add this to buff lua? {
                        
                        pe:AddParticle(self,'rumble_shield_01.troy') --Applied to why so mad
                        self:ApplyBuff(2,'rumbleshieldbuff',1.000000) --Stacks: 1, Visible: 0
                        pe:AddParticle(self,'rumble_shield_speed_buf.troy') --Applied to why so mad
                        pe:AddParticle(self,'rumble_shield_speed_buf_booster.troy') --Applied to why so mad
                }
                }]]
        elseif spellIndex == 2 then 
                self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4
                --.. more work to do
        elseif spellIndex == 3 then 
                self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4
        end
end