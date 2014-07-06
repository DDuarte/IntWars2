function CastSpell(self,spellIndex,x,y)
        if not self.isMorphed then
        if spellIndex == 2 then 
                SendMissile(self,'EliseHumanE',x,y)--,0x210)
        elseif spellIndex == 3 then 
        	self.model = 'EliseSpider'
        end
        else
                if spellIndex >= 0 and spellIndex < 3 then
                self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4
                 end
               if spellIndex == 3 then 
                        self.model = self.type
                end

        end
end