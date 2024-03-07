// ai_cast_characters.c
//

#include "g_local.h"
#include "../game/botlib.h"      //bot lib interface
#include "../game/be_aas.h"
#include "../game/be_ea.h"
#include "../game/be_ai_gen.h"
#include "../game/be_ai_goal.h"
#include "../game/be_ai_move.h"
#include "botai.h"          //bot ai interface

#include "ai_cast.h"

std::vector<AICharacterDefaults_t> aiDefaults;

/*
=========================
G_WeaponNameToId
=========================
*/
weapon_t G_WeaponNameToId(const std::string& name) {
	if (name == "WP_KNIFE") return WP_KNIFE;
	else if (name == "WP_LUGER") return WP_LUGER;
	else if (name == "WP_MP40") return WP_MP40;
	else if (name == "WP_MAUSER") return WP_MAUSER;
	else if (name == "WP_FG42") return WP_FG42;
	else if (name == "WP_GRENADE_LAUNCHER") return WP_GRENADE_LAUNCHER;
	else if (name == "WP_PANZERFAUST") return WP_PANZERFAUST;
	else if (name == "WP_VENOM") return WP_VENOM;
	else if (name == "WP_FLAMETHROWER") return WP_FLAMETHROWER;
	else if (name == "WP_TESLA") return WP_TESLA;
	else if (name == "WP_COLT") return WP_COLT;
	else if (name == "WP_THOMPSON") return WP_THOMPSON;
	else if (name == "WP_GARAND") return WP_GARAND;
	else if (name == "WP_GRENADE_PINEAPPLE") return WP_GRENADE_PINEAPPLE;
	else if (name == "WP_SNIPERRIFLE") return WP_SNIPERRIFLE;
	else if (name == "WP_SNOOPERSCOPE") return WP_SNOOPERSCOPE;
	else if (name == "WP_FG42SCOPE") return WP_FG42SCOPE;
	else if (name == "WP_STEN") return WP_STEN;
	else if (name == "WP_SILENCER") return WP_SILENCER;
	else if (name == "WP_AKIMBO") return WP_AKIMBO;
	else if (name == "WP_CLASS_SPECIAL") return WP_CLASS_SPECIAL;
	else if (name == "WP_DYNAMITE") return WP_DYNAMITE;
	else if (name == "WP_MONSTER_ATTACK1") return WP_MONSTER_ATTACK1;
	else if (name == "WP_MONSTER_ATTACK2") return WP_MONSTER_ATTACK2;
	else if (name == "WP_MONSTER_ATTACK3") return WP_MONSTER_ATTACK3;
	else if (name == "WP_GAUNTLET") return WP_GAUNTLET;
	else if (name == "WP_SNIPER") return WP_SNIPER;
	else if (name == "WP_GRENADE_SMOKE") return WP_GRENADE_SMOKE;
	else if (name == "WP_MEDIC_HEAL") return WP_MEDIC_HEAL;
	else if (name == "WP_MORTAR") return WP_MORTAR;
	else if (name == "VERYBIGEXPLOSION") return VERYBIGEXPLOSION;

	trap_Error("Unknown AI weapon id\n");
	// Default case if the weapon is not found
	return WP_NONE;
}

/*
=========================
G_WeaponNamesToBitwiseId
=========================
*/
int G_WeaponNamesToBitwiseId(const std::string& names) {
	auto tokens = split(names, '|'); // Assuming '|' is used to separate weapon names in the input string
	int combinedWeaponId = 0;

	for (const auto& name : tokens) {
		weapon_t weaponId = G_WeaponNameToId(name);
		combinedWeaponId |= static_cast<int>(weaponId);
	}

	return combinedWeaponId;
}

/*
=========================
G_StringToAiState
=========================
*/
aistateEnum_t G_StringToAiState(const std::string& stateName) {
	if (stateName == "AISTATE_RELAXED") return AISTATE_RELAXED;
	else if (stateName == "AISTATE_QUERY") return AISTATE_QUERY;
	else if (stateName == "AISTATE_ALERT") return AISTATE_ALERT;
	else if (stateName == "AISTATE_COMBAT") return AISTATE_COMBAT;

	// Default case if the AI state is not found, assuming RELAXED as a safe default
	trap_Error("Unknown AI state id\n");
	return AISTATE_RELAXED;
}

void *GetAiFunctionByName(const std::string& functionName) {
	if (functionName.size() <= 0)
		return nullptr;

	static const std::map<std::string, void *> aiFunctionMap = {
		{"AIFunc_ZombieFlameAttackStart", AIFunc_ZombieFlameAttackStart},
		{"AIFunc_ZombieAttack2Start", AIFunc_ZombieAttack2Start},
		{"AIFunc_ZombieMeleeStart", AIFunc_ZombieMeleeStart},
		{"AIFunc_WarriorZombieMeleeStart", AIFunc_WarriorZombieMeleeStart},
		{"AIFunc_WarriorZombieDefenseStart", AIFunc_WarriorZombieDefenseStart},
		{"AIFunc_StimSoldierAttack2Start", AIFunc_StimSoldierAttack2Start},
		{"AIFunc_StimSoldierAttack1Start", AIFunc_StimSoldierAttack1Start},
		{"AIFunc_BlackGuardAttack1Start", AIFunc_BlackGuardAttack1Start},
		{"AIFunc_Helga_MeleeStart", AIFunc_Helga_MeleeStart},
		{"AIFunc_Helga_SpiritAttack_Start", AIFunc_Helga_SpiritAttack_Start},
		{"AIFunc_Heinrich_MeleeStart", AIFunc_Heinrich_MeleeStart},
		{"AIFunc_Heinrich_RaiseDeadStart", AIFunc_Heinrich_RaiseDeadStart},
		{"AIFunc_Heinrich_SpawnSpiritsStart", AIFunc_Heinrich_SpawnSpiritsStart},
		{"AIFunc_LoperAttack2Start", AIFunc_LoperAttack2Start},
		{"AIFunc_LoperAttack3Start", AIFunc_LoperAttack3Start},
		// Add more mappings as needed
	};

	auto it = aiFunctionMap.find(functionName);
	if (it != aiFunctionMap.end()) {
		return it->second;
	}
	else {
		// Handle the case where the function name does not exist.
		trap_Error("Unknown GetAiFunctionByName id\n");
		return nullptr;
	}
}

/*
=========================
G_AiTeamNameToId
=========================
*/
AITeam_t G_AiTeamNameToId(const std::string& name) {
	if (name == "AITEAM_NAZI") return AITEAM_NAZI;
	else if (name == "AITEAM_ALLIES") return AITEAM_ALLIES;
	else if (name == "AITEAM_MONSTER") return AITEAM_MONSTER;
	else if (name == "AITEAM_SPARE1") return AITEAM_SPARE1;
	else if (name == "AITEAM_SPARE2") return AITEAM_SPARE2;
	else if (name == "AITEAM_SPARE3") return AITEAM_SPARE3;
	else if (name == "AITEAM_SPARE4") return AITEAM_SPARE4;
	else if (name == "AITEAM_NEUTRAL") return AITEAM_NEUTRAL;
	// Default case if the team is not found
	trap_Error("Unknown AI team id\n");
	return AITEAM_NEUTRAL; // Using NEUTRAL as a default/fallback, adjust as necessary
}

/*
=========================
G_AiFlagsNameToBitwiseId
=========================
*/
unsigned int G_AiFlagsNameToBitwiseId(const std::string& names) {
	auto tokens = split(names, '|');
	unsigned int combinedFlags = 0;

	if (names == "0" || names.size() <= 0)
		return 0;

	for (const auto& name : tokens) {
		if (name == "AIFL_CATCH_GRENADE") combinedFlags |= AIFL_CATCH_GRENADE;
		else if (name == "AIFL_NO_FLAME_DAMAGE") combinedFlags |= AIFL_NO_FLAME_DAMAGE;
		else if (name == "AIFL_FIRED") combinedFlags |= AIFL_FIRED;
		else if (name == "AIFL_LAND_ANIM_PLAYED") combinedFlags |= AIFL_LAND_ANIM_PLAYED;
		else if (name == "AIFL_ROLL_ANIM") combinedFlags |= AIFL_ROLL_ANIM;
		else if (name == "AIFL_FLIP_ANIM") combinedFlags |= AIFL_FLIP_ANIM;
		else if (name == "AIFL_STAND_IDLE2") combinedFlags |= AIFL_STAND_IDLE2;
		else if (name == "AIFL_NOAVOID") combinedFlags |= AIFL_NOAVOID;
		else if (name == "AIFL_NOPAIN") combinedFlags |= AIFL_NOPAIN;
		else if (name == "AIFL_WALKFORWARD") combinedFlags |= AIFL_WALKFORWARD;
		else if (name == "AIFL_DENYACTION") combinedFlags |= AIFL_DENYACTION;
		else if (name == "AIFL_VIEWLOCKED") combinedFlags |= AIFL_VIEWLOCKED;
		else if (name == "AIFL_CORPSESIGHTING") combinedFlags |= AIFL_CORPSESIGHTING;
		else if (name == "AIFL_WAITINGTOSPAWN") combinedFlags |= AIFL_WAITINGTOSPAWN;
		else if (name == "AIFL_JUST_SPAWNED") combinedFlags |= AIFL_JUST_SPAWNED;
		else if (name == "AIFL_NO_RELOAD") combinedFlags |= AIFL_NO_RELOAD;
		else if (name == "AIFL_TALKING") combinedFlags |= AIFL_TALKING;
		else if (name == "AIFL_NO_HEADLOOK") combinedFlags |= AIFL_NO_HEADLOOK;
		else if (name == "AIFL_ATTACK_CROUCH") combinedFlags |= AIFL_ATTACK_CROUCH;
		else if (name == "AIFL_MISCFLAG1") combinedFlags |= AIFL_MISCFLAG1;
		else if (name == "AIFL_MISCFLAG2") combinedFlags |= AIFL_MISCFLAG2;
		else if (name == "AIFL_ZOOMING") combinedFlags |= AIFL_ZOOMING;
		else if (name == "AIFL_NO_HEADSHOT_DMG") combinedFlags |= AIFL_NO_HEADSHOT_DMG;
		else if (name == "AIFL_DIVE_ANIM") combinedFlags |= AIFL_DIVE_ANIM;
		else if (name == "AIFL_NO_TESLA_DAMAGE") combinedFlags |= AIFL_NO_TESLA_DAMAGE;
		else if (name == "AIFL_EXPLICIT_ROUTING") combinedFlags |= AIFL_EXPLICIT_ROUTING;
		else if (name == "AIFL_DISMOUNTING") combinedFlags |= AIFL_DISMOUNTING;
		else if (name == "AIFL_SPECIAL_FUNC") combinedFlags |= AIFL_SPECIAL_FUNC;
		else trap_Error("Unknown AIFL id\n");
	}

	return combinedFlags;
}

/*
=========================
G_BboxTypeNameToEnum
=========================
*/
BBoxType_t G_BboxTypeNameToEnum(const std::string& name) {
	if (name == "BBOX_SMALL") return BBOX_SMALL;
	else if (name == "BBOX_LARGE") return BBOX_LARGE;
	trap_Error("Unknown bbox id\n");
	return BBOX_SMALL;
}

/*
=========================
G_ParseCharacterTable
=========================
*/
void G_ParseCharacterTable(char* data) {
	std::istringstream dataStream(data);
	std::string line;

	// AICHAR_NONE
	AICharacterDefaults_t blankCharacter = {};
	aiDefaults.push_back(blankCharacter);

	// Skip the first line if it's a header
	std::getline(dataStream, line);

	while (std::getline(dataStream, line)) {
		std::istringstream lineStream(line);
		std::string field;
		std::vector<std::string> fields;

		while (std::getline(lineStream, field, ',')) {
			fields.push_back(field);
		}

		// Check and remove trailing \r from the last field, if present
		if (!fields.empty() && !fields.back().empty() && fields.back().back() == '\r') {
			fields.back().erase(fields.back().size() - 1);
		}

		if (!fields.empty()) {
			// Assuming the first line is a header and should be skipped or fields.size() > a certain number to validate data rows
			AICharacterDefaults_t character;

			// Initialize character from fields here as in previous examples
			// Attributes directly mapped
			character.attributes[0] = std::stof(fields[2]);  // RunningSpeed
			character.attributes[1] = std::stof(fields[3]);  // WalkingSpeed
			character.attributes[2] = std::stof(fields[4]);  // CrouchingSpeed
			character.attributes[3] = std::stof(fields[5]);  // FieldOfView
			character.attributes[4] = std::stof(fields[6]);  // YawSpeed
			character.attributes[5] = std::stof(fields[7]);  // Leader
			character.attributes[6] = std::stof(fields[8]);  // AimSkill
			character.attributes[7] = std::stof(fields[9]);  // AimAccuracy
			character.attributes[8] = std::stof(fields[10]); // AttackSkill
			character.attributes[9] = std::stof(fields[11]); // ReactionTime
			character.attributes[10] = std::stof(fields[12]); // AttackCrouch
			character.attributes[11] = std::stof(fields[13]); // IdleCrouch
			character.attributes[12] = std::stof(fields[14]); // Aggression
			character.attributes[13] = std::stof(fields[15]); // Tactical
			character.attributes[14] = std::stof(fields[16]); // Camper
			character.attributes[15] = std::stof(fields[17]); // Alertness
			character.attributes[16] = std::stof(fields[18]); // StartingHealth
			character.attributes[17] = std::stof(fields[19]); // HearingScale
			character.attributes[18] = std::stof(fields[20]); // NotInPvsHearingScale
			character.attributes[19] = std::stof(fields[21]); // RelaxedDetectionRadius
			character.attributes[20] = std::stof(fields[22]); // PainThresholdMultiplier

			// Initialize sound scripts
			character.soundScripts[0] = strdup(fields[23].c_str());
			character.soundScripts[1] = strdup(fields[24].c_str());
			character.soundScripts[2] = strdup(fields[25].c_str());
			character.soundScripts[3] = strdup(fields[26].c_str());
			character.soundScripts[4] = strdup(fields[27].c_str());
			character.soundScripts[5] = strdup(fields[28].c_str());
			character.soundScripts[6] = strdup(fields[29].c_str());
			character.soundScripts[7] = strdup(fields[30].c_str());
			character.soundScripts[8] = strdup(fields[31].c_str());
			character.soundScripts[9] = fields.size() > 32 ? strdup(fields[32].c_str()) : nullptr; // Assuming 10 sound scripts based on the initial description

			// AI Team and skin
			character.aiTeam = G_AiTeamNameToId(fields[33]);
			character.skin = strdup(fields[34].c_str());

			// Weapons
			character.weapons[0] = G_WeaponNamesToBitwiseId(fields[35]);
			character.weapons[1] = G_WeaponNamesToBitwiseId(fields[36]);
			character.weapons[2] = G_WeaponNamesToBitwiseId(fields[37]);
			character.weapons[3] = G_WeaponNamesToBitwiseId(fields[38]); // Adjust based on the actual number of weapon fields in your CSV

			// BboxType, CrouchHeight, StandHeight, AiFlags
			character.bboxType = G_BboxTypeNameToEnum(fields[39]);
			character.crouchstandZ[0] = std::stof(fields[40]); // CrouchHeight
			character.crouchstandZ[1] = std::stof(fields[41]); // StandHeight
			character.aiFlags = G_AiFlagsNameToBitwiseId(fields[42]); // AiFlags

			// Function pointers for AI attack functions.
			character.aifuncAttack1 = (char* (__cdecl*)(cast_state_s*))GetAiFunctionByName(fields[43]);
			character.aifuncAttack2 = (char* (__cdecl*)(cast_state_s*))GetAiFunctionByName(fields[44]);
			character.aifuncAttack3 = (char* (__cdecl*)(cast_state_s*))GetAiFunctionByName(fields[45]);

			// Looping sound and AI state
			character.loopingSound = strdup(fields[46].c_str());
			character.aiState = G_StringToAiState(fields[47]); // Convert string to enum value

			// Add the character to the global vector
			aiDefaults.push_back(character);
		}
	}
}

/*
=========================
G_InitCharacterTable
=========================
*/
void G_InitCharacterTable(void) {
	char* tableBuffer;
	if (trap_FS_ReadFile("tables/aicast.csv", (void **)&tableBuffer) <= 0) {
		trap_Error("Failed to load aicast table!");
	}

	G_ParseCharacterTable(tableBuffer);

	trap_FS_FreeFile(tableBuffer);
}

//---------------------------------------------------------------------------
// Bounding boxes
static vec3_t bbmins[2] = {{-18, -18, -24},{-32,-32,-24}};
static vec3_t bbmaxs[2] = {{ 18,  18,  48},{ 32, 32, 68}};
//static float crouchMaxZ[2] = {32,48};	// same as player, will head be ok? // TTimo: unused
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Weapon info
cast_weapon_info_t weaponInfo;
//---------------------------------------------------------------------------

/*
============
AIChar_SetBBox

  FIXME: pass a maxZ into this so we can tailor the height for each character,
  since height isn't important for the AAS routing (whereas width is very important)
============
*/
void AIChar_SetBBox( gentity_t *ent, cast_state_t *cs, qboolean useHeadTag ) {
	vec3_t bbox[2];
	trace_t tr;
	orientation_t or;

	if ( !useHeadTag ) {
		VectorCopy( bbmins[cs->aasWorldIndex], ent->client->ps.mins );
		VectorCopy( bbmaxs[cs->aasWorldIndex], ent->client->ps.maxs );
		ent->client->ps.maxs[2] = aiDefaults[cs->aiCharacter].crouchstandZ[1];
		VectorCopy( ent->client->ps.mins, ent->r.mins );
		VectorCopy( ent->client->ps.maxs, ent->r.maxs );
		ent->client->ps.crouchMaxZ = aiDefaults[cs->aiCharacter].crouchstandZ[0];
		ent->s.density = cs->aasWorldIndex;
	} else if ( trap_GetTag( ent->s.number, "tag_head", &or ) ) {  // if not found, then just leave it
		or.origin[2] -= ent->client->ps.origin[2];  // convert to local coordinates
		or.origin[2] += 11;
		if ( or.origin[2] < 0 ) {
			or.origin[2] = 0;
		}
		if ( or.origin[2] > aiDefaults[cs->aiCharacter].crouchstandZ[1] + 30 ) {
			or.origin[2] = aiDefaults[cs->aiCharacter].crouchstandZ[1] + 30;
		}

		memset( &tr, 0, sizeof( tr ) );

		// check that the new height is ok first, otherwise leave it alone
		VectorCopy( bbmins[cs->aasWorldIndex], bbox[0] );
		VectorCopy( bbmaxs[cs->aasWorldIndex], bbox[1] );
		// set the head tag height
		bbox[1][2] = or.origin[2];

		if ( bbox[1][2] > ent->client->ps.maxs[2] ) {
			// check this area is clear
			trap_TraceCapsule( &tr, ent->client->ps.origin, bbox[0], bbox[1], ent->client->ps.origin, ent->s.number, ent->clipmask );
		}

		if ( !tr.startsolid && !tr.allsolid ) {
			VectorCopy( bbox[0], ent->client->ps.mins );
			VectorCopy( bbox[1], ent->client->ps.maxs );
			VectorCopy( ent->client->ps.mins, ent->r.mins );
			VectorCopy( ent->client->ps.maxs, ent->r.maxs );
			ent->client->ps.crouchMaxZ = aiDefaults[cs->aiCharacter].crouchstandZ[0];
			ent->s.density = cs->aasWorldIndex;
		}
	}

	// if they are linked, then relink to update bbox
	if ( ent->r.linked ) {
		trap_LinkEntity( ent );
	}
}

/*
============
AIChar_Death
============
*/
void AIChar_Death( gentity_t *ent, gentity_t *attacker, int damage, int mod ) { //----(SA)	added mod
	// need this check otherwise sound will overwrite gib message
	if ( ent->health > GIB_HEALTH  ) {
		if ( ent->client->ps.eFlags & EF_HEADSHOT ) {
			G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( aiDefaults[ent->aiCharacter].soundScripts[QUIETDEATHSOUNDSCRIPT] ) );
		} else {
			switch ( mod ) {               //----(SA)	modified to add 'quiet' deaths
			case MOD_KNIFE_STEALTH:
			case MOD_SNIPERRIFLE:
			case MOD_SNOOPERSCOPE:
				G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( aiDefaults[ent->aiCharacter].soundScripts[QUIETDEATHSOUNDSCRIPT] ) );
				break;
			case MOD_FLAMETHROWER:
				G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( aiDefaults[ent->aiCharacter].soundScripts[FLAMEDEATHSOUNDSCRIPT] ) );      //----(SA)	added
				break;
			default:
				G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( aiDefaults[ent->aiCharacter].soundScripts[DEATHSOUNDSCRIPT] ) );
				break;
			}
		}
	}
}

/*
=============
AIChar_GetPainLocation
=============
*/
int AIChar_GetPainLocation( gentity_t *ent, vec3_t point ) {
	static char *painTagNames[] = {
		"tag_head",
		"tag_chest",
		"tag_torso",
		"tag_groin",
		"tag_armright",
		"tag_armleft",
		"tag_legright",
		"tag_legleft",
		NULL,
	};

	int tagIndex, bestTag;
	float bestDist, dist;
	orientation_t or;

	// first make sure the client is able to retrieve tag information
	if ( !trap_GetTag( ent->s.number, painTagNames[0], &or ) ) {
		return 0;
	}

	// find a correct animation to play, based on the body orientation at previous frame
	for ( tagIndex = 0, bestDist = 0, bestTag = -1; painTagNames[tagIndex]; tagIndex++ ) {
		// grab the tag with this name
		if ( trap_GetTag( ent->s.number, painTagNames[tagIndex], &or ) ) {
			dist = VectorDistance( or.origin, point );
			if ( !bestDist || dist < bestDist ) {
				bestTag = tagIndex;
				bestDist = dist;
			}
		}
	}

	if ( bestTag >= 0 ) {
		return bestTag + 1;
	}

	return 0;
}

/*
============
AIChar_Pain
============
*/
void AIChar_Pain( gentity_t *ent, gentity_t *attacker, int damage, vec3_t point ) {
	#define PAIN_THRESHOLD      25
	#define STUNNED_THRESHOLD   30
	cast_state_t    *cs;
	float dist;
	qboolean forceStun = qfalse;
	float painThreshold, stunnedThreshold;

	cs = AICast_GetCastState( ent->s.number );

	if ( g_testPain.integer == 1 ) {
		ent->health = ent->client->pers.maxHealth;  // debugging
	}

	if ( g_testPain.integer != 2 ) {
		if ( level.time < cs->painSoundTime ) {
			return;
		}
	}

	painThreshold = PAIN_THRESHOLD * cs->attributes[PAIN_THRESHOLD_SCALE];
	stunnedThreshold = STUNNED_THRESHOLD * cs->attributes[PAIN_THRESHOLD_SCALE];

	// if they are already playing another animation, we might get confused and cut it off, so don't play a pain
	if ( ent->client->ps.torsoTimer || ent->client->ps.legsTimer ) {
		return;
	}

	// if we are waiting for our weapon to fire (throwing a grenade)
	if ( ent->client->ps.weaponDelay ) {
		return;
	}

	if ( attacker->s.weapon == WP_FLAMETHROWER && !( cs->aiFlags & AIFL_NO_FLAME_DAMAGE ) ) {   // flames should be recognized more often, since they stay onfire until they're dead anyway
		painThreshold = 1;
		stunnedThreshold = 99999;   // dont be stunned
	}

	// HACK: if the attacker is using the flamethrower, don't do any special pain anim or sound
	// FIXME: we should pass in the MOD here, since they could have fired a grenade, then switched weapons
	//if (attacker->s.weapon == WP_FLAMETHROWER) {
	//	return;
	//}

	if ( !Q_stricmp( attacker->classname, "props_statue" ) ) {
		damage = 99999; // try and force a stun
		forceStun = qtrue;
	}

	if ( attacker->s.weapon == WP_TESLA ) {
		damage *= 2;
		if ( cs->attributes[PAIN_THRESHOLD_SCALE] <= 1.0 ) {
			damage = 99999;
		}
	}

	// now check the damageQuota to see if we should play a pain animation
	// first reduce the current damageQuota with time
	if ( cs->damageQuotaTime && cs->damageQuota > 0 ) {
		cs->damageQuota -= (int)( ( 1.0 + ( g_gameskill.value / GSKILL_MAX ) ) * ( (float)( level.time - cs->damageQuotaTime ) / 1000 ) * ( 7.5 + cs->attributes[ATTACK_SKILL] * 10.0 ) );
		if ( cs->damageQuota < 0 ) {
			cs->damageQuota = 0;
		}
	}

	// if it's been a long time since our last pain, scale it up
	if ( cs->painSoundTime < level.time - 1000 ) {
		float scale;
		scale = (float)( level.time - cs->painSoundTime - 1000 ) / 1000.0;
		if ( scale > 4.0 ) {
			scale = 4.0;
		}
		damage = (int)( (float)damage * ( 1.0 + ( scale * ( 1.0 - 0.5 * g_gameskill.value / GSKILL_MAX ) ) ) );
	}

	// adjust the new damage with distance, if they are really close, scale it down, to make it
	// harder to get through the game by continually rushing the enemies
	if ( ( attacker->s.weapon != WP_TESLA ) && ( ( dist = VectorDistance( ent->r.currentOrigin, attacker->r.currentAngles ) ) < 384 ) ) {
		damage -= (int)( (float)damage * ( 1.0 - ( dist / 384.0 ) ) * ( 0.5 + 0.5 * g_gameskill.value / GSKILL_MAX ) );
	}

	// add the new damage
	cs->damageQuota += damage;
	cs->damageQuotaTime = level.time;

	if ( forceStun ) {
		damage = 99999; // try and force a stun
		cs->damageQuota = painThreshold + 1;
	}

	// if it's over the threshold, play a pain

	// don't do this if crouching, or we might clip through the world

	if ( g_testPain.integer == 2 || ( cs->damageQuota > painThreshold ) ) {
		int delay;

		// stunned?
		if ( damage > stunnedThreshold && ( forceStun || ( rand() % 2 ) ) ) {   // stunned
			BG_UpdateConditionValue( ent->s.number, ANIM_COND_STUNNED, qtrue, qfalse );
		}
		// enemy weapon
		if ( attacker->client ) {
			BG_UpdateConditionValue( ent->s.number, ANIM_COND_ENEMY_WEAPON, attacker->s.weapon, qtrue );
		}
		if ( point ) {
			// location
			BG_UpdateConditionValue( ent->s.number, ANIM_COND_IMPACT_POINT, AIChar_GetPainLocation( ent, point ), qtrue );
		} else {
			BG_UpdateConditionValue( ent->s.number, ANIM_COND_IMPACT_POINT, 0, qfalse );
		}

		// pause while we play a pain
		delay = BG_AnimScriptEvent( &ent->client->ps, ANIM_ET_PAIN, qfalse, qtrue );

		// turn off temporary conditions
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_STUNNED, 0, qfalse );
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_ENEMY_WEAPON, 0, qfalse );
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_IMPACT_POINT, 0, qfalse );

		if ( delay >= 0 ) {
			// setup game stuff to handle the character movements, etc
			cs->pauseTime = level.time + delay + 250;
			cs->lockViewAnglesTime = cs->pauseTime;
			// make sure we stop crouching
			cs->attackcrouch_time = 0;
			// don't fire while in pain?
			cs->triggerReleaseTime = cs->pauseTime;
			// stay crouching if we were before the pain
			if ( cs->bs->cur_ps.viewheight == cs->bs->cur_ps.crouchViewHeight ) {
				cs->attackcrouch_time = level.time + (float)( cs->pauseTime - level.time ) + 500;
			}
		}

		// if we didnt just play a scripted sound, then play one of the default sounds
		if ( cs->lastScriptSound < level.time ) {
			G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( aiDefaults[ent->aiCharacter].soundScripts[PAINSOUNDSCRIPT] ) );
		}

		// reset the quota
		cs->damageQuota = 0;
		cs->damageQuotaTime = 0;
		//
		cs->painSoundTime = cs->pauseTime + (int)( 1000 * ( g_gameskill.value / GSKILL_MAX ) );     // add a bit more of a buffer before the next one
	}

}

/*
============
AIChar_Sight
============
*/
void AIChar_Sight( gentity_t *ent, gentity_t *other, int lastSight ) {
	cast_state_t    *cs;

	cs = AICast_GetCastState( ent->s.number );

	// if we are in noattack mode, don't make sounds
	if ( cs->castScriptStatus.scriptNoAttackTime >= level.time ) {
		return;
	}
	if ( cs->noAttackTime >= level.time ) {
		return;
	}

	// if they have recently played a script sound, then ignore this
	if ( cs->lastScriptSound > level.time - 4000 ) {
		return;
	}

	if ( !AICast_SameTeam( cs, other->s.number ) ) {
		if ( !cs->firstSightTime || cs->firstSightTime < ( level.time - 15000 ) ) {
			//G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( aiDefaults[ent->aiCharacter].sightSoundScript ) );
		}
		cs->firstSightTime = level.time;
	}

}

/*
=====================
AIChar_AttackSND

  NOTE: this should just lookup a sound script for this character/weapon combo
=====================
*/
void AIChar_AttackSound( cast_state_t *cs ) {

	gentity_t *ent;

	ent = &g_entities [cs->entityNum];

	if ( cs->attackSNDtime > level.time ) {
		return;
	}

	// if we are in noattack mode, don't make sounds
	if ( cs->castScriptStatus.scriptNoAttackTime >= level.time ) {
		return;
	}
	if ( cs->noAttackTime >= level.time ) {
		return;
	}

	// Ridah, only yell when throwing grenades every now and then, since it's not very "stealthy"
	if ( cs->weaponNum == WP_GRENADE_LAUNCHER && rand() % 5 ) {
		return;
	}

	cs->attackSNDtime = level.time + 5000 + ( 1000 * rand() % 10 );

	AICast_ScriptEvent( cs, "attacksound", ent->aiName );
	if ( cs->aiFlags & AIFL_DENYACTION ) {
		return;
	}

	if ( cs->weaponNum == WP_LUGER ) {
		G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( aiDefaults[ent->aiCharacter].soundScripts[ORDERSSOUNDSCRIPT] ) );
	} else {
		G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( aiDefaults[ent->aiCharacter].soundScripts[ATTACKSOUNDSCRIPT] ) );
	}

}

/*
============
AIChar_spawn
============
*/
void AIChar_spawn( gentity_t *ent ) {
	gentity_t       *newent;
	cast_state_t    *cs;
	AICharacterDefaults_t *aiCharDefaults;
	int i;
	static int lastCall;
	static int numCalls;

	// if there are other cast's waiting to spawn before us, wait for them
	for ( i = MAX_CLIENTS, newent = &g_entities[MAX_CLIENTS]; i < MAX_GENTITIES; i++, newent++ ) {
		if ( !newent->inuse ) {
			continue;
		}
		if ( newent->think != AIChar_spawn ) {
			continue;
		}
		if ( newent == ent ) {
			break;      // we are the first in line
		}
		// still waiting for someone else
		ent->nextthink = level.time + FRAMETIME;
		return;
	}

	// if the client hasn't connected yet, wait around
	if ( !AICast_FindEntityForName( "player" ) ) {
		ent->nextthink = level.time + FRAMETIME;
		return;
	}

	if ( lastCall == level.time ) {
		if ( numCalls++ > 2 ) {
			ent->nextthink = level.time + FRAMETIME;
			return;     // spawned enough this frame already
		}
	} else {
		numCalls = 0;
	}
	lastCall = level.time;

	aiCharDefaults = &aiDefaults[ent->aiCharacter];

	// ............................
	// setup weapon info
	//
	// starting weapons/ammo
	memset( &weaponInfo, 0, sizeof( weaponInfo ) );
	for ( i = 0; aiCharDefaults->weapons[i]; i++ ) {
		//weaponInfo.startingWeapons[(aiCharDefaults->weapons[i] / 32)] |= ( 1 << aiCharDefaults->weapons[i] );
		//weaponInfo.startingWeapons[0] |= ( 1 << aiCharDefaults->weapons[i] );

		COM_BitSet( weaponInfo.startingWeapons, aiCharDefaults->weapons[i] );
		if ( aiCharDefaults->weapons[i] == WP_GRENADE_LAUNCHER ) { // give them a bunch of grenades, but not an unlimited supply
			weaponInfo.startingAmmo[BG_FindAmmoForWeapon( (weapon_t)aiCharDefaults->weapons[i] )] = 6;
		} else {
			weaponInfo.startingAmmo[BG_FindAmmoForWeapon((weapon_t)aiCharDefaults->weapons[i] )] = 999;
		}
	}
	//
	// use the default skin if nothing specified
	if ( !ent->aiSkin || !strlen( ent->aiSkin ) ) {
		ent->aiSkin = aiCharDefaults->skin;
	}
	// ............................
	//
	// create the character

	// (there will always be an ent->aiSkin (SA))
	newent = AICast_CreateCharacter( ent, aiCharDefaults->attributes, &weaponInfo, aiCharDefaults->name, ent->aiSkin, ent->aihSkin, "m", "7", "100" );

	if ( !newent ) {
		G_FreeEntity( ent );
		return;
	}
	// copy any character-specific information to the new entity (like editor fields, etc)
	//
	// copy this across so killing ai can trigger a target
	newent->target = ent->target;
	//
	newent->classname = ent->classname;
	newent->r.svFlags |= ( ent->r.svFlags & SVF_NOFOOTSTEPS );
	newent->aiCharacter = ent->aiCharacter;
	newent->client->ps.aiChar = ent->aiCharacter;
	newent->spawnflags = ent->spawnflags;
	newent->aiTeam = ent->aiTeam;
	if ( newent->aiTeam < 0 ) {
		newent->aiTeam = aiCharDefaults->aiTeam;
	}
	newent->client->ps.teamNum = newent->aiTeam;
	//
	// kill the old entity
	G_FreeEntity( ent );
	// attach to the new entity
	ent = newent;
	//
	// precache any specific sounds
	//
	// ...
	//
	// get the cast state
	cs = AICast_GetCastState( ent->s.number );
	//
	// setup any character specific cast_state variables
	cs->deathfunc = AIChar_Death;
	cs->painfunc = AIChar_Pain;
	cs->aiFlags |= aiCharDefaults->aiFlags;
	cs->aiState = aiCharDefaults->aiState;
	//
	cs->queryCountValidTime = -1;
	//
	// randomly choose idle animation
	if ( cs->aiFlags & AIFL_STAND_IDLE2 ) {
		newent->client->ps.eFlags |= EF_STAND_IDLE2;
	}
	//
	// attach any event specific functions (pain, death, etc)
	//
	//cs->getDeathAnim = AIChar_getDeathAnim;
	cs->sightfunc = AIChar_Sight;
	if ( ent->aiTeam == AITEAM_ALLIES || ent->aiTeam == AITEAM_NEUTRAL ) { // friendly
		cs->activate = AICast_ProcessActivate;
	} else {
		cs->activate = NULL;
	}
	cs->aifuncAttack1 = aiCharDefaults->aifuncAttack1;
	cs->aifuncAttack2 = aiCharDefaults->aifuncAttack2;
	cs->aifuncAttack3 = aiCharDefaults->aifuncAttack3;
	//
	// looping sound?
	if ( aiCharDefaults->loopingSound ) {
		ent->s.loopSound = G_SoundIndex( aiCharDefaults->loopingSound );
	}
	//
	// precache sounds for this character
	for ( i = 0; i < MAX_AI_EVENT_SOUNDS; i++ ) {
		if ( aiDefaults[ent->aiCharacter].soundScripts[i] ) {
			G_SoundIndex( aiDefaults[ent->aiCharacter].soundScripts[i] );
		}
	}
	//
	if ( ent->aiCharacter == AICHAR_HEINRICH ) {
		AICast_Heinrich_SoundPrecache();
	}
	//
	// special spawnflag stuff
	if ( ent->spawnflags & 2 ) {
		cs->secondDeadTime = qtrue;
	}
	//
	// init scripting
	cs->castScriptStatus.castScriptEventIndex = -1;
	cs->castScriptStatus.scriptAttackEnt = -1;
	//
	// set crouch move speed
	ent->client->ps.crouchSpeedScale = cs->attributes[CROUCHING_SPEED] / cs->attributes[RUNNING_SPEED];
	//
	// check for some anims which we can use for special behaviours
	if ( BG_GetAnimScriptEvent( &ent->client->ps, ANIM_ET_ROLL ) >= 0 ) {
		cs->aiFlags |= AIFL_ROLL_ANIM;
	}
	if ( BG_GetAnimScriptEvent( &ent->client->ps, ANIM_ET_FLIP ) >= 0 ) {
		cs->aiFlags |= AIFL_FLIP_ANIM;
	}
	if ( BG_GetAnimScriptEvent( &ent->client->ps, ANIM_ET_DIVE ) >= 0 ) {
		cs->aiFlags |= AIFL_DIVE_ANIM;
	}
	// HACK
	if ( ent->aiName && !Q_stricmp( ent->aiName, "deathshead" ) ) {
		cs->aiFlags |= AIFL_NO_FLAME_DAMAGE;
	}
	//
	// check for no headshot damage
	if ( cs->aiFlags & AIFL_NO_HEADSHOT_DMG ) {
		ent->headshotDamageScale = 0.0;
	}
	// set these values now so scripting system isn't relying on a Think having been run prior to running a script
	//origin of the cast
	VectorCopy( ent->client->ps.origin, cs->bs->origin );
	//velocity of the cast
	VectorCopy( ent->client->ps.velocity, cs->bs->velocity );
	//playerstate
	cs->bs->cur_ps = ent->client->ps;
	//
	if ( !ent->aiInactive ) {
		// trigger a spawn script event
		AICast_ScriptEvent( cs, "spawn", "" );
	} else {
		trap_UnlinkEntity( ent );
	}

}

//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_soldier (1 0.25 0) (-16 -16 -24) (16 16 64) TriggerSpawn NoRevive
soldier entity
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'infantryss/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/
/*
-------- MODEL FOR RADIANT ONLY - DO NOT SET THIS AS A KEY --------
model="models\mapobjects\characters\test\nazi.md3"
*/
/*
============
SP_ai_soldier
============
*/
void SP_ai_soldier( gentity_t *ent ) {
	AICast_DelayedSpawnCast( ent, AICHAR_SOLDIER );
}

//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_american (1 0.25 0) (-16 -16 -24) (16 16 64) TriggerSpawn NoRevive
american entity
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'american/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_american
============
*/
void SP_ai_american( gentity_t *ent ) {
	AICast_DelayedSpawnCast( ent, AICHAR_AMERICAN );
}

//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_zombie (1 0.25 0) (-16 -16 -24) (16 16 64) TriggerSpawn NoRevive PortalZombie
zombie entity
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'zombie/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_zombie
============
*/
void SP_ai_zombie( gentity_t *ent ) {
	ent->r.svFlags |= SVF_NOFOOTSTEPS;
	AICast_DelayedSpawnCast( ent, AICHAR_ZOMBIE );
}


//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_warzombie (1 0.25 0) (-16 -16 -24) (16 16 64) TriggerSpawn NoRevive PortalZombie
warrior zombie entity
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'warrior/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_warzombie
============
*/
void SP_ai_warzombie( gentity_t *ent ) {
	AICast_DelayedSpawnCast( ent, AICHAR_WARZOMBIE );
}


//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_venom (1 0.25 0) (-16 -16 -24) (16 16 64) TriggerSpawn NoRevive
venom entity
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'venom/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_venom
============
*/
void SP_ai_venom( gentity_t *ent ) {
	ent->r.svFlags |= SVF_NOFOOTSTEPS;
	AICast_DelayedSpawnCast( ent, AICHAR_VENOM );
}


//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_loper (1 0.25 0) (-32 -32 -24) (32 32 48) TriggerSpawn NoRevive
loper entity
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'loper/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_loper
============
*/
void SP_ai_loper( gentity_t *ent ) {
	ent->r.svFlags |= SVF_NOFOOTSTEPS;
	AICast_DelayedSpawnCast( ent, AICHAR_LOPER );
	//
	level.loperZapSound = G_SoundIndex( "loperZap" );
}

//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_boss_helga (1 0.25 0) (-16 -16 -24) (16 16 64) TriggerSpawn NoRevive
helga entity
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'helga/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_boss_helga
============
*/
void SP_ai_boss_helga( gentity_t *ent ) {
	AICast_DelayedSpawnCast( ent, AICHAR_HELGA );
}

//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_boss_heinrich (1 0.25 0) (-32 -32 -24) (32 32 156) TriggerSpawn NoRevive
heinrich entity
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'helga/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_boss_heinrich
============
*/
void SP_ai_boss_heinrich( gentity_t *ent ) {
	AICast_DelayedSpawnCast( ent, AICHAR_HEINRICH );
}

//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_partisan (1 0.25 0) (-16 -16 -24) (16 16 64) TriggerSpawn NoRevive
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'partisan/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_partisan
============
*/
void SP_ai_partisan( gentity_t *ent ) {
	AICast_DelayedSpawnCast( ent, AICHAR_PARTISAN );
}

//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_civilian (1 0.25 0) (-16 -16 -24) (16 16 64) TriggerSpawn NoRevive
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'civilian/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_civilian
============
*/
void SP_ai_civilian( gentity_t *ent ) {
	AICast_DelayedSpawnCast( ent, AICHAR_CIVILIAN );
}

//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_eliteguard (1 0.25 0) (-16 -16 -24) (16 16 64) TriggerSpawn NoRevive
elite guard entity
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'eliteguard/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_eliteguard
============
*/
void SP_ai_eliteguard( gentity_t *ent ) {
	AICast_DelayedSpawnCast( ent, AICHAR_ELITEGUARD );
}


//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_frogman (1 0.25 0) (-16 -16 -24) (16 16 64) TriggerSpawn NoRevive
elite guard entity
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'frogman/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_frogman
============
*/
void SP_ai_frogman( gentity_t *ent ) {
	ent->r.svFlags |= SVF_NOFOOTSTEPS;
	AICast_DelayedSpawnCast( ent, AICHAR_FROGMAN );
}


//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_stimsoldier_dual (1 0.25 0) (-32 -32 -24) (32 32 64) TriggerSpawn NoRevive
stim soldier entity
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'stim/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_stimsoldier_dual
============
*/
void SP_ai_stimsoldier_dual( gentity_t *ent ) {
	AICast_DelayedSpawnCast( ent, AICHAR_STIMSOLDIER1 );
	//
	level.stimSoldierFlySound = G_SoundIndex( "sound/stimsoldier/flyloop.wav" );
}

//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_stimsoldier_rocket (1 0.25 0) (-32 -32 -24) (32 32 64) TriggerSpawn NoRevive
stim soldier entity
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'stim/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_stimsoldier_rocket
============
*/
void SP_ai_stimsoldier_rocket( gentity_t *ent ) {
	AICast_DelayedSpawnCast( ent, AICHAR_STIMSOLDIER2 );
	//
	level.stimSoldierFlySound = G_SoundIndex( "sound/stimsoldier/flyloop.wav" );
}

//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_stimsoldier_tesla (1 0.25 0) (-32 -32 -24) (32 32 64) TriggerSpawn NoRevive
stim soldier entity
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'stim/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_stimsoldier_tesla
============
*/
void SP_ai_stimsoldier_tesla( gentity_t *ent ) {
	AICast_DelayedSpawnCast( ent, AICHAR_STIMSOLDIER3 );
	//
	level.stimSoldierFlySound = G_SoundIndex( "sound/stimsoldier/flyloop.wav" );
}

//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_supersoldier (1 0.25 0) (-32 -32 -24) (32 32 64) TriggerSpawn NoRevive
supersoldier entity
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'supersoldier/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_supersoldier
============
*/
void SP_ai_supersoldier( gentity_t *ent ) {
	AICast_DelayedSpawnCast( ent, AICHAR_SUPERSOLDIER );
}

//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_protosoldier (1 0.25 0) (-32 -32 -24) (32 32 64) TriggerSpawn NoRevive
protosoldier entity
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'protosoldier/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_protosoldier
============
*/
void SP_ai_protosoldier( gentity_t *ent ) {
	AICast_DelayedSpawnCast( ent, AICHAR_PROTOSOLDIER );
}

//----------------------------------------------------------------------------------------------------------------------------
/*QUAKED ai_blackguard (1 0.25 0) (-16 -16 -24) (16 16 64) TriggerSpawn NoRevive
black guard entity
"skin" the .skin file to use for this character (must exist in the player characters directory, otherwise 'blackguard/default' is used)
"head" the .skin file to use for his head (must exist in the pc's dir, otherwise 'default' is used)
"ainame" name of AI
*/

/*
============
SP_ai_blackguard
============
*/
void SP_ai_blackguard( gentity_t *ent ) {
	AICast_DelayedSpawnCast( ent, AICHAR_BLACKGUARD );
}