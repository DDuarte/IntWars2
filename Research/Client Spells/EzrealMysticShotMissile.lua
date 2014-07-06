NotSingleTargetSpell = false
DoesntTriggerSpellCasts = true
IsDamagingSpell = true

BuffTextureName = "FallenAngel_DarkBinding.dds"
BuffName = "Dark Binding"
AutoBuffActivateEffect = "DarkBinding_tar.troy"
PopupMessage1 = "game_floatingtext_Snared"

SpellFXOverrideSkins = {'CyberEzreal'}


local R1 = {Function = BBPreloadParticle}
R1["Params"] = {Name = 'ezreal_mysticshot_tar.troy'}

local R2 = {Function = BBPreloadSpell}
R1["Params"] = {Name = 'ezrealrisingspellforce'}

PreLoadBuildingBlocks = {R1,R2}