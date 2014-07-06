function CastSpell(self,spellIndex,x,y)
        --print('Spell1: '..self:GetVar('Data','Spell1'))
        --print('Spell: '..self:GetSpell(spellIndex+1))
        if not self.isMorphed then
                if spellIndex == 0 then 
                        spell = CSpellWork(self,spellIndex,x,y) --JavelinToss
                        spell.iUpdatePos = 0
                        spell.fVisible = 2.0
                        --spell.stop = true --after 0.15, send stop move
                        spell.fReleaseCast = 0.85 --release cast after 0.85 seconds?
                        
                        spell.cooldown = 0
                        spell.manaCost = 0

                        self:QueueSpell(spell)
        	elseif spellIndex == 1 then 
                        --self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4
                        --DoDash3(self,x,y,0.5)
                        --self:ApplyBuff(2,1,'InternalTestBuff',13.0)
                elseif spellIndex == 2 then 
                        spell = CSpellWork(self,spellIndex,x,y) --JavelinToss
                        spell.animation = true

                        print('mana: '..spell.manaCost)
                        print('cd: '..spell.cooldown)
                        self:QueueSpell(spell)
                        --SendSpell(self,'Spell2',1,0)
                elseif spellIndex == 3 then 
                        self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4
                        self.model = 'Nidalee_Cougar'
                end
        else
        	if spellIndex == 0 then 
                        self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4
        		--B4 missing
        		SendSpell(self,'Spell1',1,0)
        	elseif spellIndex == 1 then 
                        self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4
        		--B4 missing
        		SendSpell(self,'Spell2',1,0)
        	elseif spellIndex == 2 then 
                        self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4
        		--B4 missing
        		SendSpell(self,'Spell3',1,0)
        	elseif spellIndex == 3 then 
        		self.model = self.type
        	end
	end
end