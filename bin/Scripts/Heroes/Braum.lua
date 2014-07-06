function CastSpell(self,spellIndex,x,y)
        if spellIndex >= 0 and spellIndex <=3 then
                self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4
        end
        if spellIndex == 1 then 
                SendSpell(self,'Spell2',5,0)
        end
end