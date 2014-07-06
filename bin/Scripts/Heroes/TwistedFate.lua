function CastSpell(self,spellIndex,x,y)
	if spellIndex == 0 then 
		m = CMissile('WildCards')
        m.toX = x
        m.toY = y
        
        m.fromX = self.x;
        m.fromY = self.y;
        m.channelTime = 0.25;
        m.visible = 1.0;

        SendMissile(self,m)
		--SendSpell(self,'Spell1',5,0)
	end
end