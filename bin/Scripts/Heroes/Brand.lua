function CastSpell(self,spellIndex,x,y)
	if spellIndex == 0 then 
                SendMissile(self,'BrandBlaze',x,y,0x110);
		SendSpell(self,'Spell1',5,0)
	elseif spellIndex == 1 then 
                SendMissile(self,'BrandFissure',x,y,0x110);
		--SendSpell(self,'Spell2',5,0)
	end
end