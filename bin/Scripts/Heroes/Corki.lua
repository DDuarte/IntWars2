function CastSpell(self,spellIndex,x,y)
	if spellIndex == 0 then 
		m = CMissile('PhosphorusBomb')
        m.toX = x
        m.toY = y
        
        m.fromX = self.x;
        m.fromY = self.y;
        m.channelTime = 0.25;
        m.visible = 1.0;

        SendMissile(self,m)
		--SendSpell(self,'Spell1',5,0)
	elseif spellIndex == 1 then 
		m = CMissile('CarpetBomb')
        m.toX = x
        m.toY = y
        
        m.fromX = self.x;
        m.fromY = self.y;
        m.channelTime = 0.25;
        m.visible = 1.0;

        SendMissile(self,m)
		--SendSpell(self,'Spell2',5,0)
        elseif spellIndex == 2 then 
                m = CMissile('GGun')
        m.toX = x
        m.toY = y
        
        m.fromX = self.x;
        m.fromY = self.y;
        m.channelTime = 0.25;
        m.visible = 1.0;

        SendMissile(self,m)
                SendSpell(self,'Spell3',5,0)
        elseif spellIndex == 3 then 
                --[[m = CMissile('MissileBarrage')
        m.toX = x
        m.toY = y
        
        m.fromX = self.x;
        m.fromY = self.y;
        m.channelTime = 0.25;
        m.visible = 1.0;

        SendMissile(self,m)]]
		SendSpell(self,'Spell4',5,0)
	end
end