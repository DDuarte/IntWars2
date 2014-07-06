function CastSpell(self,spellIndex,x,y)
	if spellIndex == 0 then 
		--self:CastSpell(spellIndex,x,y);
		--6D?
		spell = CSpellWork(self,_Q,x,y) 
        spell.updatePos = 0
        spell.stop = true
        spell.releaseCast = true
        spell:AddBuff(2,'blindmonkpassive_cosmetic',3.0,2) --[   172] self:ApplyBuff(2,'blindmonkpassive_cosmetic',3.000000) --Stacks: 2, Visible: 0
                
                self:QueueSpell(spell)
                --[  1984] self:SetSpellName(1,'BlindMonkWOne') ? reset if did not hit
	elseif spellIndex == 1 then 
		spell = CSpellWork(self,_W,x,y) 
		spell:AddBuff(2,'blindmonkwoneshield',4.0,1) --4 seconds, 1 stack
                self:QueueSpell(spell)
        --[     0] self:SetSpellName(1,'BlindMonkWTwo')
                
                --[    31] self:ApplyBuff(2,'blindmonkwoneshield',4.000000) --Stacks: 1, Visible: 0
                
	elseif spellIndex == 2 then 
		spell = CSpellWork(self,_E,x,y) --EzrealMysticShot
        spell.stop = true--[    15] MoveAns(self,...) --[1] STOP
                
                p1 = spell:AddParticle(self,self,'blindMonk_thunderCrash_impact_02.troy',0.25)
                p1 = spell:AddParticle(self,self,'blindMonk_thunderCrash_impact_cas.troy',0.25)
                p1 = spell:AddParticle(self,self,'blindMonk_E_cas.troy',0.25)
                self:QueueSpell(spell)
	elseif spellIndex == 3 then 
		self:CastSpell(_R,x,y);
	end
end