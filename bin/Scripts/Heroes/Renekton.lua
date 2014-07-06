function CastSpell(self,spellIndex,x,y)
        if spellIndex == 0 then 
                spell = CSpellWork(spellIndex,'RenektonCleave',x,y) --RenektonCleave
                --spell.lock = true -- lock hero during spell?

                spell.animation = 'Spell1'
                spell:AddParticle(self,'RenektonCleave_trail.troy',0)
                
                self:QueueSpell(spell)
        elseif spellIndex == 1 then 
                spell = CSpellWork(spellIndex,'RenektonPreExecute',x,y) --RenektonPreExecute
                --spell.lock = true -- lock hero during spell?
                self:QueueSpell(spell)
                self:ApplyBuff(2,1,'RenektonPreExecute',6.000000)
        elseif spellIndex == 2 then 
                --spell = CSpellWork(spellIndex,'RenektonSliceAndDice',x,y) --RenektonSliceAndDice
                --spell.setFacing = true

                --spell.animation = 'Spell3'
                --self:DoDash(...) --63
                --spell:AddParticle(self,'RenektonSliceDice_buf.troy',0)

                --self:QueueSpell(spell)
                self:CastSpell(spellIndex,x,y)
                --self:SetPosition(...) --4F sets face position
                --self:AnimateSpell('Spell3',0.000000) -- AF
                --self:DoDash(...) --63
        elseif spellIndex == 3 then 
                self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4
        end
end