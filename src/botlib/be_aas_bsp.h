// be_aas_bsp.h
//

#define MAX_EPAIRKEY        128

/**
 * Traces through the world.
 *
 * @param start Start position of the trace.
 * @param mins Minimum bounds of the trace area.
 * @param maxs Maximum bounds of the trace area.
 * @param end End position of the trace.
 * @param passent The passing entity that should be ignored in the trace.
 * @param contentmask The mask that specifies which types of content should be considered by the trace.
 * @return Information about the trace result.
 */
bsp_trace_t AAS_Trace(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int passent, int contentmask);

/**
 * Returns the contents at the given point.
 *
 * @param point The point to check.
 * @return The contents at the point.
 */
int AAS_PointContents(vec3_t point);

/**
 * Determines if p2 is in the Potential Visibility Set (PVS) of p1.
 *
 * @param p1 First point.
 * @param p2 Second point.
 * @return True if p2 is in the PVS of p1, false otherwise.
 */
qboolean AAS_inPVS(vec3_t p1, vec3_t p2);

/**
 * Determines if p2 is in the Potential Hearing Set (PHS) of p1.
 *
 * @param p1 First point.
 * @param p2 Second point.
 * @return True if p2 is in the PHS of p1, false otherwise.
 */
qboolean AAS_inPHS(vec3_t p1, vec3_t p2);

/**
 * Checks if two areas are connected.
 *
 * @param area1 First area.
 * @param area2 Second area.
 * @return True if the given areas are connected, false otherwise.
 */
qboolean AAS_AreasConnected(int area1, int area2);

/**
 * Creates a list of entities totally or partly within the given box.
 *
 * @param absmins Minimum bounds of the box.
 * @param absmaxs Maximum bounds of the box.
 * @param list Output list of entities.
 * @param maxcount Maximum number of entities to list.
 * @return The number of entities listed.
 */
int AAS_BoxEntities(vec3_t absmins, vec3_t absmaxs, int* list, int maxcount);

/**
 * Gets the minimums, maximums, and origin of a BSP model.
 *
 * @param modelnum The model number.
 * @param angles The angles of the model.
 * @param mins Output minimum bounds of the model.
 * @param maxs Output maximum bounds of the model.
 * @param origin Output origin of the model.
 */
void AAS_BSPModelMinsMaxsOrigin(int modelnum, vec3_t angles, vec3_t mins, vec3_t maxs, vec3_t origin);

/**
 * Gets a handle to the next BSP entity.
 *
 * @param ent The current entity.
 * @return The next BSP entity.
 */
int AAS_NextBSPEntity(int ent);

/**
 * Returns the value of the BSP epair key.
 *
 * @param ent The entity.
 * @param key The key to look for.
 * @param value Output value for the key.
 * @param size Size of the value buffer.
 * @return Status of the operation.
 */
int AAS_ValueForBSPEpairKey(int ent, char* key, char* value, int size);

/**
 * Gets a vector for the BSP epair key.
 *
 * @param ent The entity.
 * @param key The key to look for.
 * @param v Output vector.
 * @return Status of the operation.
 */
int AAS_VectorForBSPEpairKey(int ent, char* key, vec3_t v);

/**
 * Gets a float for the BSP epair key.
 *
 * @param ent The entity.
 * @param key The key to look for.
 * @param value Output float value.
 * @return Status of the operation.
 */
int AAS_FloatForBSPEpairKey(int ent, char* key, float* value);

/**
 * Gets an integer for the BSP epair key.
 *
 * @param ent The entity.
 * @param key The key to look for.
 * @param value Output integer value.
 * @return Status of the operation.
 */
int AAS_IntForBSPEpairKey(int ent, char* key, int* value);

