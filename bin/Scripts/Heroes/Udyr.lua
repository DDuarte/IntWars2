function CastSpell(self,spellIndex,x,y)
	if spellIndex == 0 then 
		self.model = 'UdyrTiger'
	elseif spellIndex == 1 then 
		self.model = 'UdyrTurtle'
	elseif spellIndex == 2 then 
		self.model = 'Udyr'
	--[[elseif spellIndex == 2 then 
		if self.isMorphed then
			self.model = self.type
		else
			self.model = 'UdyrBear'
		end]]
	elseif spellIndex == 3 then 
		self.model = 'UdyrPhoenix'
	end
end