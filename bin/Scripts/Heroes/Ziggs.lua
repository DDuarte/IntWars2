function CastSpell(self,spellIndex,x,y)
	if spellIndex == 0 then 
                self:CastSpell(spellIndex,x,y)
		--SendSpell(self,'Spell1',5,0)
	elseif spellIndex == 1 then 
                self:CastSpell(spellIndex,x,y)
		--SendSpell(self,'Spell2',5,0)
        elseif spellIndex == 2 then 
                self:CastSpell(spellIndex,x,y)
                --SendSpell(self,'Spell3',5,0)
        elseif spellIndex == 3 then 
                self:CastSpell(spellIndex,x,y)
                --SendSpell(self,'Spell3',5,0)
	end
end