function CastSpell(self,spellIndex)
	local bMorpth = self.model ~= self.type
	if spellIndex == 3 then 
		if bMorpth then
			self.model = self.type
		else
			--self.model = 'Jayce2?'
		end
	end
end