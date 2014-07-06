function CastSpell(self,spellIndex,x,y)
	if spellIndex == 0 then 
		if shared.qIndex == nil then shared.qIndex = 1 end -- index nil to 1
		if shared.qIndex == 1 then --First Q
			--if 'SpellData','CantCancelWhileWindingUp' then actionstate, remove move end
			self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4

			--ApplyBuff(self,5,2,1,1,0x08608245,0x7C4CAC39,3.0)--APPLY BUFF B6
			--28
			AnimateSpell(self,'Spell1a',1,0.0) --AF
			--APPLY BUFF B6
			--SetSpellName(self,0,'RivenTriCleaveBuffer') --17
			--SetupSpell1 84
			--SetupSpell1 84
			--APPLY BUFF B6
			--86 - NO NETID HEADER
			--ActionState 4000006 (no move?)
			--Dash 63  - NO NETID HEADER
			--37
			--86 - NO NETID HEADER
			
			--[[ NEXT SECOND ]]
			--28
			--SetSpellName(self,0,'RivenTriCleave') --17
			--ActionState 4000007 (move?)
			shared.qIndex = 1--2
		elseif shared.qIndex == 2 then
			self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4
			--UPDATE BUFF  1C
			--28
			AnimateSpell(self,'Spell1b',1,0.0) --AF
			--UPDATE BUFF 1C
			--SetSpellName(self,0,'RivenTriCleaveBuffer') --17
			--SetupSpell1 84
			--SetupSpell1 84
			--86 - NO NETID HEADER
			--Dash 63  - NO NETID HEADER
			--ActionState 4000006 (no move?)
			--APPLY BUFF B6
			--37
			--86 - NO NETID HEADER
			
			--[[ NEXT SECOND ]]
			--28
			--SetSpellName(self,0,'RivenTriCleave') --17
			--ActionState 4000007 (move?)
			shared.qIndex = 3
		elseif shared.qIndex == 3 then
			self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4
			--UPDATE BUFF  1C
			--28
			AnimateSpell(self,'Spell1c',1,0.0) --AF
			--UPDATE BUFF  1C
			--SetSpellName(self,0,'RivenTriCleave') --17
			--SetupSpell1 84
			--APPLY BUFF B6
			--86 - NO NETID HEADER
			--Dash 63  - NO NETID HEADER
			--7A LOSE BUFF
			--ActionState 4000006 (no move?)
			--37
			--86 - NO NETID HEADER
			
			--[[ NEXT SECOND ]]
			--28
			--ActionState 4000007 (move?)
			--3x 7A 2 seconds spaced ,maybe? LOSE BUFF
			shared.qIndex = 1
		end
	elseif spellIndex == 1 then
		self:CastSpell(spellIndex,x,y)--DisplaySpell() --B4

		self:stop() --60 Move Ans 
		pe = ParticleEmiter()
		pe:AddParticle(self,'Riven_Base_W_Cast.troy')
		pe:AddParticle(self,'exile_W_weapon_cas.troy')
		peID = pe:AddParticle(self,59)
		pe:Send(self) --returns particle emiterID / 86 PKT_FX_Create_Group  
		KillParticle(self,peID) --37 PKT_FX_Kill
		ApplyBuff(self,1,2,1,0,'RivenPassiveAABoost',5.0)
		--self:ApplyBuff(2,'RivenPassiveAABoost',5.0)--ApplyBuff(self,1,2,1,0,'RivenPassiveAABoost',5.0)--sef:ApplyBuff(...)--B6 Apply Buff --runnic blade empowerment?
		--7A 5sec later lose buff

	end
end