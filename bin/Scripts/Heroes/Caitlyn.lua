function CastSpell(self,spellIndex,x,y)
	if spellIndex == 0 then 
		SendMissile(self,'CaitlynPiltoverPeacemaker',x,y,0x110);
	elseif spellIndex == 1 then 
		SendMissile(self,'CaitlynYordleTrap',x,y,0x110);
	elseif spellIndex == 2 then 
		SendMissile(self,'CaitlynEntrapment',x,y,0x110);
		--SendSpell(self,'Spell3',5,0)
	end
end