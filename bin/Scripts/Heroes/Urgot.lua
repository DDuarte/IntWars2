function CastSpell(self,spellIndex,x,y)
	if spellIndex == 0 then 
		SendSpell(self,'Spell1',5,0)
	elseif spellIndex == 1 then 
		SendSpell(self,'Spell2',5,0)
	elseif spellIndex == 0x82 then 
		m = CMissile('UrgotPlasmaGrenade')
		m.toX = x
        m.toY = y
        
        m.fromX = self.x;
        m.fromY = self.y;
        --??
        m.f1 = 0.25;
        m.f2 = 0;
        m.f3 = 1.0;
        m.f4 = 18.0;
        m.dw15 = 0x110;
        --bounding box?
        m.f5 = 80.0;
        m.f6 = 35.902802;
        m.f7 = 211.908005;
        m.f8 = 273.551910;

        SendMissile(self,m)
		--SendSpell(self,'Spell3',5,0)
	end
end