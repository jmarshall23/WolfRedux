// be_aas_bspq3.c
//

#include "../game/q_shared.h"
#include "../botutil/l_memory.h"
#include "../botutil/l_script.h"
#include "../botutil/l_precomp.h"
#include "../botutil/l_struct.h"
#include "aasfile.h"
#include "../game/botlib.h"
#include "../game/be_aas.h"
#include "be_aas_funcs.h"
#include "be_aas_def.h"

extern botlib_import_t botimport;

//#define TRACE_DEBUG

#define ON_EPSILON      0.005
//#define DEG2RAD( a ) (( a * M_PI ) / 180.0F)

#define MAX_BSPENTITIES     2048

typedef struct rgb_s
{
	int red;
	int green;
	int blue;
} rgb_t;

//bsp entity epair
typedef struct bsp_epair_s
{
	char *key;
	char *value;
	struct bsp_epair_s *next;
} bsp_epair_t;

//bsp data entity
typedef struct bsp_entity_s
{
	bsp_epair_t *epairs;
} bsp_entity_t;

//id Sofware BSP data
typedef struct bsp_s
{
	//true when bsp file is loaded
	int loaded;
	//entity data
	int entdatasize;
	char *dentdata;
	//bsp entities
	int numentities;
	bsp_entity_t entities[MAX_BSPENTITIES];
	//memory used for strings and epairs
	byte *ebuffer;
} bsp_t;

//global bsp
bsp_t bspworld;

/**
 * Performs a trace (raycast) through the game world with an axial bounding box.
 *
 * @param start The starting position of the trace.
 * @param mins The minimum bounds of the bounding box relative to the start.
 * @param maxs The maximum bounds of the bounding box relative to the start.
 * @param end The ending position of the trace.
 * @param passent The entity number to ignore during the trace or -1 for none.
 * @param contentmask The content mask that specifies which types of objects to consider in the trace.
 * @return The result of the trace in a bsp_trace_t structure.
 */
bsp_trace_t AAS_Trace(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int passent, int contentmask) {
	bsp_trace_t bsptrace;
	botimport.Trace(&bsptrace, start, mins, maxs, end, passent, contentmask);
	return bsptrace;
}

/**
 * Returns the content flags at a given point in the game world.
 *
 * @param point The point in the game world to check.
 * @return The content flags at the specified point.
 */
int AAS_PointContents(vec3_t point) {
	return botimport.PointContents(point);
}

/**
 * Checks for a collision along a path between two points for a specific entity, considering the entity's bounding box.
 *
 * @param entnum The entity number to test the collision for.
 * @param start The starting point of the path.
 * @param boxmins The minimum bounds of the entity's bounding box.
 * @param boxmaxs The maximum bounds of the entity's bounding box.
 * @param end The ending point of the path.
 * @param contentmask The content mask that specifies which types of objects to consider in the collision test.
 * @param trace The trace result where information about the collision will be stored.
 * @return qtrue if a collision is detected before reaching the end point, otherwise qfalse.
 */
qboolean AAS_EntityCollision(int entnum, vec3_t start, vec3_t boxmins, vec3_t boxmaxs, vec3_t end, int contentmask, bsp_trace_t* trace) {
	bsp_trace_t enttrace;

	botimport.EntityTrace(&enttrace, start, boxmins, boxmaxs, end, entnum, contentmask);
	if (enttrace.fraction < trace->fraction) {
		memcpy(trace, &enttrace, sizeof(bsp_trace_t));
		return qtrue;
	}
	return qfalse;
}

/**
 * Checks if two points are in the potentially visible set (PVS), meaning they can potentially see each other.
 *
 * @param p1 The first point to test.
 * @param p2 The second point to test.
 * @return qtrue if the two points are potentially visible to each other, otherwise qfalse.
 */
qboolean AAS_inPVS(vec3_t p1, vec3_t p2) {
	return botimport.inPVS(p1, p2);
}

/**
 * Checks if two points are in the potentially hearable set (PHS), meaning sounds from one point can potentially be heard at the other.
 *
 * @param p1 The first point to test.
 * @param p2 The second point to test.
 * @return qtrue if sounds at one point can potentially be heard at the other, otherwise qfalse.
 */
qboolean AAS_inPHS(vec3_t p1, vec3_t p2) {
	return qtrue; // Placeholder implementation.
}

/**
 * Retrieves the minimum and maximum bounds and origin of a BSP model.
 *
 * @param modelnum The model number to retrieve the data for.
 * @param angles The angles to consider for the model's orientation.
 * @param mins The minimum bounds of the model will be stored here.
 * @param maxs The maximum bounds of the model will be stored here.
 * @param origin The origin of the model will be stored here.
 */
void AAS_BSPModelMinsMaxsOrigin(int modelnum, vec3_t angles, vec3_t mins, vec3_t maxs, vec3_t origin) {
	botimport.BSPModelMinsMaxsOrigin(modelnum, angles, mins, maxs, origin);
}

void AAS_UnlinkFromBSPLeaves( bsp_link_t *leaves ) {
} 


bsp_link_t *AAS_BSPLinkEntity( vec3_t absmins, vec3_t absmaxs, int entnum, int modelnum ) {
	return NULL;
} 

/**
 * Finds entities within a specified bounding box.
 *
 * @param absmins The minimum bounds of the search box.
 * @param absmaxs The maximum bounds of the search box.
 * @param list An array to store the entity numbers found within the box.
 * @param maxcount The maximum number of entities to store in the list.
 * @return The number of entities found.
 */
int AAS_BoxEntities(vec3_t absmins, vec3_t absmaxs, int* list, int maxcount) {
	return 0; // Placeholder implementation.
}

/**
 * Iterates through BSP entities in the world.
 *
 * @param ent The current entity number.
 * @return The next entity number, or 0 if there are no more entities.
 */
int AAS_NextBSPEntity(int ent) {
	ent++;
	if (ent >= 1 && ent < bspworld.numentities) {
		return ent;
	}
	return 0;
}

/**
 * Checks if a BSP entity number is within the valid range.
 *
 * @param ent The entity number to check.
 * @return qtrue if the entity number is valid, otherwise qfalse.
 */
int AAS_BSPEntityInRange(int ent) {
	if (ent <= 0 || ent >= bspworld.numentities) {
		botimport.Print(PRT_MESSAGE, "BSP entity out of range\n");
		return qfalse;
	}
	return qtrue;
}

/**
 * Retrieves the string value for a given key from a BSP entity's key-value pairs.
 *
 * @param ent The entity number.
 * @param key The key for which the value is sought.
 * @param value A buffer to store the retrieved value.
 * @param size The size of the buffer.
 * @return qtrue if the key was found and the value was successfully retrieved; otherwise, qfalse.
 */
int AAS_ValueForBSPEpairKey(int ent, char* key, char* value, int size) {
	bsp_epair_t* epair;

	value[0] = '\0';
	if (!AAS_BSPEntityInRange(ent)) {
		return qfalse;
	}
	for (epair = bspworld.entities[ent].epairs; epair; epair = epair->next) {
		if (!strcmp(epair->key, key)) {
			strncpy(value, epair->value, size - 1);
			value[size - 1] = '\0';
			return qtrue;
		}
	}
	return qfalse;
}

/**
 * Retrieves a vector value for a given key from a BSP entity's key-value pairs.
 *
 * @param ent The entity number.
 * @param key The key for which the vector value is sought.
 * @param v A vector to store the retrieved value.
 * @return qtrue if the key was found and the vector value was successfully retrieved; otherwise, qfalse.
 */
int AAS_VectorForBSPEpairKey(int ent, char* key, vec3_t v) {
	char buf[MAX_EPAIRKEY];
	double v1, v2, v3;

	VectorClear(v);
	if (!AAS_ValueForBSPEpairKey(ent, key, buf, MAX_EPAIRKEY)) {
		return qfalse;
	}
	// Parse the string into three double values, then assign them to the vector
	sscanf(buf, "%lf %lf %lf", &v1, &v2, &v3);
	v[0] = (float)v1;
	v[1] = (float)v2;
	v[2] = (float)v3;
	return qtrue;
}

/**
 * Retrieves a float value for a given key from a BSP entity's key-value pairs.
 *
 * @param ent The entity number.
 * @param key The key for which the float value is sought.
 * @param value A pointer to store the retrieved value.
 * @return qtrue if the key was found and the value was successfully retrieved; otherwise, qfalse.
 */
int AAS_FloatForBSPEpairKey(int ent, char* key, float* value) {
	char buf[MAX_EPAIRKEY];

	*value = 0.0f;
	if (!AAS_ValueForBSPEpairKey(ent, key, buf, MAX_EPAIRKEY)) {
		return qfalse;
	}
	*value = atof(buf);
	return qtrue;
}

/**
 * Retrieves an integer value for a given key from a BSP entity's key-value pairs.
 *
 * @param ent The entity number.
 * @param key The key for which the integer value is sought.
 * @param value A pointer to store the retrieved value.
 * @return qtrue if the key was found and the value was successfully retrieved; otherwise, qfalse.
 */
int AAS_IntForBSPEpairKey(int ent, char* key, int* value) {
	char buf[MAX_EPAIRKEY];

	*value = 0;
	if (!AAS_ValueForBSPEpairKey(ent, key, buf, MAX_EPAIRKEY)) {
		return qfalse;
	}
	*value = atoi(buf);
	return qtrue;
}

/**
 * Frees all memory allocated for BSP entities.
 */
void AAS_FreeBSPEntities(void) {
	if (bspworld.ebuffer) {
		FreeMemory(bspworld.ebuffer);
	}
	bspworld.numentities = 0;
}

/*
===========================================================================
 AAS_ParseBSPEntities
  
  Parses the BSP entities from the loaded BSP file data and stores them in a global structure.
  
  It first calculates the memory requirement by parsing the entity data, then allocates a single
  memory block for all entities and their key-value pairs to optimize memory usage.

  Modifies global `bspworld` by filling in the entities and their respective key-value pairs.
===========================================================================
*/
void AAS_ParseBSPEntities(void) {
	script_t* script;
	token_t token;
	bsp_entity_t* ent;
	bsp_epair_t* epair;
	byte* buffer, * buftrav;
	int bufsize;

	// Initialize the entity buffer pointer to NULL at the start.
	bspworld.ebuffer = NULL;

	// Load the entity data from the BSP file into a script structure for parsing.
	script = LoadScriptMemory(bspworld.dentdata, bspworld.entdatasize, "entdata");
	SetScriptFlags(script, SCFL_NOSTRINGWHITESPACES | SCFL_NOSTRINGESCAPECHARS);

	// Calculate the total size needed for all entities and their key-value pairs.
	bufsize = 0;
	while (PS_ReadToken(script, &token)) {
		if (strcmp(token.string, "{")) {
			ScriptError(script, "invalid %s\n", token.string);
			AAS_FreeBSPEntities();
			FreeScript(script);
			return;
		}
		if (bspworld.numentities >= MAX_BSPENTITIES) {
			botimport.Print(PRT_MESSAGE, "too many entities in BSP file\n");
			break;
		}
		while (PS_ReadToken(script, &token)) {
			if (!strcmp(token.string, "}")) {
				break;
			}
			bufsize += sizeof(bsp_epair_t) + 2 * (strlen(token.string) + 1); // Key and value sizes.
			if (token.type != TT_STRING || !PS_ExpectTokenType(script, TT_STRING, 0, &token)) {
				ScriptError(script, "expected string type for key or value\n");
				AAS_FreeBSPEntities();
				FreeScript(script);
				return;
			}
		}
	}
	FreeScript(script);

	// Allocate memory for all entities and their key-value pairs.
	buffer = (byte*)GetClearedHunkMemory(bufsize);
	buftrav = buffer;
	bspworld.ebuffer = buffer;

	// Parse the entities again, now storing them into the allocated memory.
	script = LoadScriptMemory(bspworld.dentdata, bspworld.entdatasize, "entdata");
	SetScriptFlags(script, SCFL_NOSTRINGWHITESPACES | SCFL_NOSTRINGESCAPECHARS);
	bspworld.numentities = 1;

	while (PS_ReadToken(script, &token)) {
		ent = &bspworld.entities[bspworld.numentities++];
		ent->epairs = NULL;
		while (PS_ReadToken(script, &token)) {
			if (!strcmp(token.string, "}")) {
				break;
			}
			// Allocate and link a new key-value pair under the current entity.
			epair = (bsp_epair_t*)buftrav; buftrav += sizeof(bsp_epair_t);
			epair->next = ent->epairs;
			ent->epairs = epair;

			// Store the key.
			StripDoubleQuotes(token.string);
			epair->key = (char*)buftrav; buftrav += (strlen(token.string) + 1);
			strcpy(epair->key, token.string);

			// Read and store the value.
			if (!PS_ExpectTokenType(script, TT_STRING, 0, &token)) {
				AAS_FreeBSPEntities();
				FreeScript(script);
				return;
			}
			StripDoubleQuotes(token.string);
			epair->value = (char*)buftrav; buftrav += (strlen(token.string) + 1);
			strcpy(epair->value, token.string);
		}
	}
	FreeScript(script);
}

/**
 * Traces light from a start point to an end point, determining the light intensity at the end point.
 *
 * @param start The starting point of the light trace.
 * @param end The ending point of the light trace.
 * @param endpos The position at which the light trace ended.
 * @param red A pointer to store the red component of the light intensity.
 * @param green A pointer to store the green component of the light intensity.
 * @param blue A pointer to store the blue component of the light intensity.
 * @return The light intensity at the end position.
 */
int AAS_BSPTraceLight(vec3_t start, vec3_t end, vec3_t endpos, int* red, int* green, int* blue) {
	// Placeholder for actual implementation.
	return 0;
}

/**
 * Clears and frees all loaded BSP data.
 */
void AAS_DumpBSPData(void) {
	AAS_FreeBSPEntities();

	if (bspworld.dentdata) {
		FreeMemory(bspworld.dentdata);
	}
	bspworld.dentdata = NULL;
	bspworld.entdatasize = 0;

	bspworld.loaded = qfalse;
	memset(&bspworld, 0, sizeof(bspworld));
}

/**
 * Loads BSP data from the game's BSP entity data.
 *
 * @return A status code indicating success or failure.
 */
int AAS_LoadBSPFile(void) {
	AAS_DumpBSPData();

	bspworld.entdatasize = strlen(botimport.BSPEntityData()) + 1;
	bspworld.dentdata = (char*)GetClearedHunkMemory(bspworld.entdatasize);

	memcpy(bspworld.dentdata, botimport.BSPEntityData(), bspworld.entdatasize);

	AAS_ParseBSPEntities();

	bspworld.loaded = qtrue;

	return BLERR_NOERROR;
}
