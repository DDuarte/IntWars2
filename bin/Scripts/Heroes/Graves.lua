function CastSpell(self,spellIndex,x,y)
	if spellIndex == 0 then 
                --[[self:SetupSpell(0) 
		self:CastSpell(spellIndex,x,y)
                --stop? MoveAns(self,...) --60
		--self:SetSpellName(51,'GravesClusterShotSoundMissile')
                CreateMissile(self,'GravesClusterShotAttack',x,y)
                CreateMissile(self,'GravesClusterShotSoundMissile',x,y)
                CreateMissile(self,'GravesClusterShotAttack',x,y)
                CreateMissile(self,'GravesClusterShotAttack',x,y)]]

                spell = CSpellWork(self,spellIndex,x,y) --Graves
                spell.stop = true
                spell.iUpdatePos = 0
                spell.cooldown = 0
                spell.manaCost = 0
                --spell.setFacing = true

                spell:AddMissile(self,'GravesClusterShotAttack',0.25)
                spell:AddMissile(self,'GravesClusterShotSoundMissile',0.25)
                spell:AddMissile(self,'GravesClusterShotAttack',0.25)
                spell:AddMissile(self,'GravesClusterShotAttack',0.25)
                
                self:QueueSpell(spell)

	elseif spellIndex == 1 then 
                --[[self:SetupSpell(1) 
                self:CastSpell(spellIndex,x,y)
		--stop? MoveAns(self,...) --60
                --self:SetPosition(...) --4F sets face position
                --self:CastSpell(spellIndex,x,y) --[0AD99EFD] NOT FOUND / 0077 flag 

                pe = ParticleEmiter()
                pe:AddParticle(self,'Graves_SmokeGrenade_Cloud_Team_Green.troy') --Applied to (null) < NOT OK - APPLY TO NULL
                pe:AddParticle(self,'Graves_SmokeGrenade_Boom.troy') --Applied to (null) < NOT OK - APPLY TO NULL
                pe:Send(self)]]
                spell = CSpellWork(self,spellIndex,x,y) --EzrealMysticShot
                spell.iUpdatePos = 1
                spell.stop = true
                spell.cooldown = 0
                spell.manaCost = 0
                --spell.setFacing = true

                spell:AddParticle(self,'Graves_SmokeGrenade_Cloud_Team_Green.troy',0.25)
                spell:AddParticle(self,'Graves_SmokeGrenade_Boom.troy',0.25)
                
                self:QueueSpell(spell)
        elseif spellIndex == 2 then 
                --[[self:CastSpell(spellIndex,x,y)
                --self:SetPosition(...) --4F sets face position / DoDash(self,0.927345,-0.094225,0.362149,0.083333)--4F ? dash

                AnimateSpell(self,'Spell3',5,0.0)
                --DoDash2(self,x,y)--63

                pe = ParticleEmiter()
                pe:AddParticle(self,'Graves_Move_OnBuffActivate.troy') --Applied to why so mad
                pe:Send(self)

                self:ApplyBuff(2,'gravesmovesteroid',4.000000) --Stacks: 1, Visible: 0]]
                spell = CSpellWork(self,spellIndex,x,y) --EzrealMysticShot
                spell.cooldown = 0
                spell.manaCost = 0
                --spell.setFacing = true (4F)
                spell.animation = true
                --dash
                spell:AddParticle(self,self,'Graves_Move_OnBuffActivate.troy',0.25)
                spell:AddBuff(2,'gravesmovesteroid',4.0,1) --Stacks: 1, Visible: 0]]
                
                self:QueueSpell(spell)

        elseif spellIndex == 3 then 
                self:CastSpell(spellIndex,x,y)
		--SendSpell(self,'Spell4',5,0)
	end
end