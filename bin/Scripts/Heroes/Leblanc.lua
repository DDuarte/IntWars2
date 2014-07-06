function CastSpell(self,spellIndex,x,y)
	print('x'..spellIndex)
	if spellIndex == 0x81 then 
		SendMissile(self,'LeblancSlide',x,y,0x110);
		--SendSpell2(self,'leblancslidereturn') 17 NETID 01 01 STRING
		SendSpell(self,'Spell2',5,0)
		--63
		--86
		--self:SetActionState(0x00400006)
		--disable spells?
	elseif spellIndex == 2 then 
		SendMissile(self,'LeblancSoulShackle',x,y)
	end
end