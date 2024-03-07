// be_aas_sample.c
//

/*
===================================================
AAS (Area Awareness System) environment sampling.

This refers to the process of collecting and analyzing data from a game's environment to build a navigational mesh (navmesh) that AI entities use for pathfinding and movement decisions.
===================================================
*/

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

//#define AAS_SAMPLE_DEBUG

#define BBOX_NORMAL_EPSILON     0.001

#define ON_EPSILON                  0 //0.0005

#define TRACEPLANE_EPSILON          0.125

typedef struct aas_tracestack_s
{
	vec3_t start;       //start point of the piece of line to trace
	vec3_t end;         //end point of the piece of line to trace
	int planenum;       //last plane used as splitter
	int nodenum;        //node found after splitting with planenum
} aas_tracestack_t;

typedef struct
{
	int nodenum;        //node found after splitting
} aas_linkstack_t;

/*
=======================================
AAS_PresenceTypeBoundingBox
	- Determines the bounding box for an entity based on its presence type (normal or crouching). This helps in understanding the physical space an entity occupies in the game world.
=======================================
*/
void AAS_PresenceTypeBoundingBox(int presencetype, vec3_t mins, vec3_t maxs) {
	int index;
	//bounding box size for each presence type
	vec3_t boxmins[3] = { {0, 0, 0}, {-15, -15, -24}, {-15, -15, -24} };
	vec3_t boxmaxs[3] = { {0, 0, 0}, { 15,  15,  32}, { 15,  15,   8} };

	if (presencetype == PRESENCE_NORMAL) {
		index = 1;
	}
	else if (presencetype == PRESENCE_CROUCH) {
		index = 2;
	}
	else
	{
		botimport.Print(PRT_FATAL, "AAS_PresenceTypeBoundingBox: unknown presence type\n");
		index = 2;
	}
	VectorCopy(boxmins[index], mins);
	VectorCopy(boxmaxs[index], maxs);
}

/*
=======================================
AAS_InitAASLinkHeap
	- Initializes a heap of AAS links, which are data structures used to efficiently manage dynamic entities' spatial information in the game world. This is crucial for the AAS to quickly access which entities are in which areas.
=======================================
*/
void AAS_InitAASLinkHeap(void) {
	int i, max_aaslinks;

	max_aaslinks = aasworld->linkheapsize;

	//if there's no link heap present
	if (!aasworld->linkheap) {
		max_aaslinks = (int)4096; //LibVarValue("max_aaslinks", "4096");
		if (max_aaslinks < 0) {
			max_aaslinks = 0;
		}
		aasworld->linkheapsize = max_aaslinks;
		aasworld->linkheap = (aas_link_t*)GetHunkMemory(max_aaslinks * sizeof(aas_link_t));
	}

	//link the links on the heap
	aasworld->linkheap[0].prev_ent = NULL;
	aasworld->linkheap[0].next_ent = &aasworld->linkheap[1];
	for (i = 1; i < max_aaslinks - 1; i++)
	{
		aasworld->linkheap[i].prev_ent = &aasworld->linkheap[i - 1];
		aasworld->linkheap[i].next_ent = &aasworld->linkheap[i + 1];
	}

	aasworld->linkheap[max_aaslinks - 1].prev_ent = &aasworld->linkheap[max_aaslinks - 2];
	aasworld->linkheap[max_aaslinks - 1].next_ent = NULL;

	//pointer to the first free link
	aasworld->freelinks = &aasworld->linkheap[0];
}

/*
=======================================
AAS_FreeAASLinkHea
	- Deallocates the AAS link heap, freeing up the memory allocated for managing the spatial information of dynamic entities. This is typically used during cleanup or when the AAS data is being reloaded.
=======================================
*/
void AAS_FreeAASLinkHeap(void) {
	if (aasworld->linkheap) {
		FreeMemory(aasworld->linkheap);
	}

	aasworld->linkheap = NULL;
	aasworld->linkheapsize = 0;
}

/*
=======================================
AAS_AllocAASLink
	- Allocates an AAS link from the link heap. These links are used to connect entities to the spatial areas they belong to, enabling the AAS to track entity movement.
=======================================
*/
aas_link_t* AAS_AllocAASLink(void) {
	aas_link_t* link;

	link = aasworld->freelinks;
	if (!link) {
		botimport.Print(PRT_FATAL, "empty aas link heap\n");
		return NULL;
	}

	if (aasworld->freelinks) {
		aasworld->freelinks = aasworld->freelinks->next_ent;
	}

	if (aasworld->freelinks) {
		aasworld->freelinks->prev_ent = NULL;
	}
	return link;
}

/*
=======================================
AAS_DeAllocAASLink
	- Deallocates an AAS link, removing an entity's connection to a spatial area. This function is used when entities move or are removed from the game world.
=======================================
*/
void AAS_DeAllocAASLink(aas_link_t* link) {
	if (aasworld->freelinks) {
		aasworld->freelinks->prev_ent = link;
	}

	link->prev_ent = NULL;
	link->next_ent = aasworld->freelinks;
	link->prev_area = NULL;
	link->next_area = NULL;
	aasworld->freelinks = link;
}

/*
=======================================
AAS_InitAASLinkedEntities
	- Initializes the data structures for linking entities to AAS areas. This setup allows the AAS to know which entities are in which areas, facilitating navigation and movement decisions.
=======================================
*/
void AAS_InitAASLinkedEntities(void) {
	if (!aasworld->loaded) {
		return;
	}

	if (aasworld->arealinkedentities) {
		FreeMemory(aasworld->arealinkedentities);
	}

	aasworld->arealinkedentities = (aas_link_t**)GetClearedHunkMemory(
		aasworld->numareas * sizeof(aas_link_t*));
}

/*
=======================================
AAS_FreeAASLinkedEntities
	- Frees the memory allocated for linking entities to AAS areas, typically used during cleanup or when the AAS data is reloaded.
=======================================
*/
void AAS_FreeAASLinkedEntities(void) {
	if (aasworld->arealinkedentities) {
		FreeMemory(aasworld->arealinkedentities);
	}
	aasworld->arealinkedentities = NULL;
}

/*
=======================================
AAS_PointAreaNum
	- Determines the AAS area number that a given point in space belongs to. This function is fundamental in navigation as it helps in locating where something (or someone) is in the game world.
=======================================
*/
int AAS_PointAreaNum(vec3_t point) {
	int nodeNum;
	vec_t distanceFromPlane;
	aas_node_t* node;
	aas_plane_t* plane;

	// Check if the AAS world data is loaded
	if (!aasworld->loaded) {
		botimport.Print(PRT_ERROR, "AAS_PointAreaNum: AAS data not loaded\n");
		return 0;
	}

	// Start traversal from node 1, node 0 is reserved for solid leafs
	nodeNum = 1;
	while (nodeNum > 0) {
		node = &aasworld->nodes[nodeNum];

#ifdef AAS_SAMPLE_DEBUG
		// Validate node number
		if (nodeNum >= aasworld->numnodes) {
			botimport.Print(PRT_ERROR, "Invalid node number: %d, exceeds total nodes: %d\n", nodeNum, aasworld->numnodes);
			return 0;
		}
		// Validate plane number		
		if (node->planenum < 0 || node->planenum >= aasworld->numplanes) {
			botimport.Print(PRT_ERROR, "Invalid plane number: %d, exceeds total planes: %d\n", node->planenum, aasworld->numplanes);
			return 0;
		}
#endif
		// Determine which side of the plane the point is on
		plane = &aasworld->planes[node->planenum];
		distanceFromPlane = DotProduct(point, plane->normal) - plane->dist;

		// Choose the next node based on the side of the plane
		nodeNum = (distanceFromPlane > 0) ? node->children[0] : node->children[1];
	}

	// If nodeNum is 0, the point is in a solid area
	if (nodeNum == 0) {
#ifdef AAS_SAMPLE_DEBUG
		botimport.Print(PRT_MESSAGE, "Point is in solid\n");
#endif
		return 0;
	}

	// Return the area number (negative value to indicate leaf node)
	return -nodeNum;
}

/*
=======================================
AAS_AreaCluster
	- Returns the cluster number of an AAS area, which is part of the spatial subdivision used by the AAS to organize the game world into manageable sections for pathfinding.
=======================================
*/
int AAS_AreaCluster(int areanum) {
	if (areanum <= 0 || areanum >= aasworld->numareas) {
		botimport.Print(PRT_ERROR, "AAS_AreaCluster: invalid area number\n");
		return 0;
	}

	return aasworld->areasettings[areanum].cluster;
}

/*
=======================================
AAS_AreaPresenceType
	- Returns the presence types allowed within a specified AAS area. This information is used to determine what kinds of entities can navigate through an area (e.g., whether it's accessible to entities in a crouching state).
=======================================
*/
int AAS_AreaPresenceType(int areanum) {
	if (!aasworld->loaded) {
		return 0;
	}

	if (areanum <= 0 || areanum >= aasworld->numareas) {
		botimport.Print(PRT_ERROR, "AAS_AreaPresenceType: invalid area number\n");
		return 0;
	}
	return aasworld->areasettings[areanum].presencetype;
}

/*
=======================================
AAS_AreaPresenceType
	- Determines the presence type at a specific point in the game world, useful for understanding navigation constraints at particular locations.
=======================================
*/
int AAS_PointPresenceType(vec3_t point) {
	int areanum;

	if (!aasworld->loaded) {
		return 0;
	}

	areanum = AAS_PointAreaNum(point);
	if (!areanum) {
		return PRESENCE_NONE;
	}
	return aasworld->areasettings[areanum].presencetype;
}

/*
=======================================
AAS_AreaEntityCollision
	- Checks for collisions between entities and the AAS areas they are trying to move through. This function is essential for ensuring that entity movement respects the physical constraints of the game world.
=======================================
*/
qboolean AAS_AreaEntityCollision(int areaNum, vec3_t start, vec3_t end, int presenceType, int passEntity, aas_trace_t* trace) {
	qboolean hasCollision = qfalse;
	vec3_t boxMins, boxMaxs;
	aas_link_t* link;
	bsp_trace_t bspTrace;

	// Calculate bounding box size based on presence type
	AAS_PresenceTypeBoundingBox(presenceType, boxMins, boxMaxs);

	// Initialize bspTrace to ensure it's clean before use
	memset(&bspTrace, 0, sizeof(bsp_trace_t));
	bspTrace.fraction = 1; // Assume no collision initially

	// Iterate over entities linked to the specified area
	for (link = aasworld->arealinkedentities[areaNum]; link; link = link->next_ent) {
		// Skip the entity specified as 'passEntity'
		if (link->entnum == passEntity) {
			continue;
		}

		// Check for collision with the entity, considering solid and player clip contents
		if (AAS_EntityCollision(link->entnum, start, boxMins, boxMaxs, end,
			CONTENTS_SOLID | CONTENTS_PLAYERCLIP, &bspTrace)) {
			hasCollision = qtrue;
			break; // Collision found, no need to check further
		}
	}

	// If a collision was detected, copy relevant information to the trace output
	if (hasCollision) {
		trace->startsolid = bspTrace.startsolid;
		trace->ent = bspTrace.ent;
		VectorCopy(bspTrace.endpos, trace->endpos);
		trace->area = 0; // No area information to provide
		trace->planenum = 0; // No plane information to provide
		return qtrue;
	}

	return qfalse; // No collision detected
}

/*
=======================================
AAS_TraceClientBBox
	- Performs a trace from one point to another considering an entity's bounding box, checking for collisions with the world geometry. This is crucial for movement and collision detection.
=======================================
*/
aas_trace_t AAS_TraceClientBBox(vec3_t start, vec3_t end, int presencetype,
	int passent) {
	int side, nodenum, tmpplanenum;
	float front, back, frac;
	vec3_t cur_start, cur_end, cur_mid, v1, v2;
	aas_tracestack_t tracestack[127];
	aas_tracestack_t* tstack_p;
	aas_node_t* aasnode;
	aas_plane_t* plane;
	aas_trace_t trace;

	//clear the trace structure
	memset(&trace, 0, sizeof(aas_trace_t));

	if (!aasworld->loaded) {
		return trace;
	}

	tstack_p = tracestack;

	//we start with the whole line on the stack
	VectorCopy(start, tstack_p->start);
	VectorCopy(end, tstack_p->end);
	tstack_p->planenum = 0;

	//start with node 1 because node zero is a dummy for a solid leaf
	tstack_p->nodenum = 1;      //starting at the root of the tree
	tstack_p++;

	while (1)
	{
		//pop up the stack
		tstack_p--;

		//if the trace stack is empty (ended up with a piece of the
		//line to be traced in an area)
		if (tstack_p < tracestack) {
			tstack_p++;
			//nothing was hit
			trace.startsolid = qfalse;
			trace.fraction = 1.0;
			//endpos is the end of the line
			VectorCopy(end, trace.endpos);
			//nothing hit
			trace.ent = 0;
			trace.area = 0;
			trace.planenum = 0;
			return trace;
		}

		//number of the current node to test the line against
		nodenum = tstack_p->nodenum;

		//if it is an area
		if (nodenum < 0) {
#ifdef AAS_SAMPLE_DEBUG
			if (-nodenum > aasworld->numareasettings) {
				botimport.Print(PRT_ERROR, "AAS_TraceBoundingBox: -nodenum out of range\n");
				return trace;
			}
#endif //AAS_SAMPLE_DEBUG
			//botimport.Print(PRT_MESSAGE, "areanum = %d, must be %d\n", -nodenum, AAS_PointAreaNum(start));
			//if can't enter the area because it hasn't got the right presence type
			if (!(aasworld->areasettings[-nodenum].presencetype & presencetype)) {
				//if the start point is still the initial start point
				//NOTE: no need for epsilons because the points will be
				//exactly the same when they're both the start point
				if (tstack_p->start[0] == start[0] &&
					tstack_p->start[1] == start[1] &&
					tstack_p->start[2] == start[2]) {
					trace.startsolid = qtrue;
					trace.fraction = 0.0;
				}
				else
				{
					trace.startsolid = qfalse;
					VectorSubtract(end, start, v1);
					VectorSubtract(tstack_p->start, start, v2);
					trace.fraction = VectorLength(v2) / VectorNormalize(v1);
					VectorMA(tstack_p->start, -0.125, v1, tstack_p->start);
				}

				VectorCopy(tstack_p->start, trace.endpos);
				trace.ent = 0;
				trace.area = -nodenum;

				//				VectorSubtract(end, start, v1);
				trace.planenum = tstack_p->planenum;

				//always take the plane with normal facing towards the trace start
				plane = &aasworld->planes[trace.planenum];
				if (DotProduct(v1, plane->normal) > 0) {
					trace.planenum ^= 1;
				}
				return trace;
			}
			else
			{
				if (passent >= 0) {
					if (AAS_AreaEntityCollision(-nodenum, tstack_p->start,
						tstack_p->end, presencetype, passent,
						&trace)) {
						if (!trace.startsolid) {
							VectorSubtract(end, start, v1);
							VectorSubtract(trace.endpos, start, v2);
							trace.fraction = VectorLength(v2) / VectorLength(v1);
						}
						return trace;
					}
				}
			} 
			trace.lastarea = -nodenum;
			continue;
		}

		//if it is a solid leaf
		if (!nodenum) {
			//if the start point is still the initial start point
			//NOTE: no need for epsilons because the points will be
			//exactly the same when they're both the start point
			if (tstack_p->start[0] == start[0] &&
				tstack_p->start[1] == start[1] &&
				tstack_p->start[2] == start[2]) {
				trace.startsolid = qtrue;
				trace.fraction = 0.0;
			}
			else
			{
				trace.startsolid = qfalse;
				VectorSubtract(end, start, v1);
				VectorSubtract(tstack_p->start, start, v2);
				trace.fraction = VectorLength(v2) / VectorNormalize(v1);
				VectorMA(tstack_p->start, -0.125, v1, tstack_p->start);
			} 
			VectorCopy(tstack_p->start, trace.endpos);
			trace.ent = 0;
			trace.area = 0; //hit solid leaf
			//			VectorSubtract(end, start, v1);
			trace.planenum = tstack_p->planenum;
			//always take the plane with normal facing towards the trace start
			plane = &aasworld->planes[trace.planenum];
			if (DotProduct(v1, plane->normal) > 0) {
				trace.planenum ^= 1;
			}
			return trace;
		}

#ifdef AAS_SAMPLE_DEBUG
		if (nodenum > aasworld->numnodes) {
			botimport.Print(PRT_ERROR, "AAS_TraceBoundingBox: nodenum out of range\n");
			return trace;
		}
#endif //AAS_SAMPLE_DEBUG

		//the node to test against
		aasnode = &aasworld->nodes[nodenum];

		//start point of current line to test against node
		VectorCopy(tstack_p->start, cur_start);

		//end point of the current line to test against node
		VectorCopy(tstack_p->end, cur_end);

		//the current node plane
		plane = &aasworld->planes[aasnode->planenum];

		switch (plane->type)
		{/*FIXME: wtf doesn't this work? obviously the axial node planes aren't always facing positive!!!
			//check for axial planes
			case PLANE_X:
			{
				front = cur_start[0] - plane->dist;
				back = cur_end[0] - plane->dist;
				break;
			} //end case
			case PLANE_Y:
			{
				front = cur_start[1] - plane->dist;
				back = cur_end[1] - plane->dist;
				break;
			} //end case
			case PLANE_Z:
			{
				front = cur_start[2] - plane->dist;
				back = cur_end[2] - plane->dist;
				break;
			} //end case*/
		default:     //gee it's not an axial plane
		{
			front = DotProduct(cur_start, plane->normal) - plane->dist;
			back = DotProduct(cur_end, plane->normal) - plane->dist;
			break;
		}     //end default
		} //end switch

		//calculate the hitpoint with the node (split point of the line)
		//put the crosspoint TRACEPLANE_EPSILON pixels on the near side
		if (front < 0) {
			frac = (front + TRACEPLANE_EPSILON) / (front - back);
		}
		else { 
			frac = (front - TRACEPLANE_EPSILON) / (front - back); 
		}

		//if the whole to be traced line is totally at the front of this node
		//only go down the tree with the front child
		if ((front >= -ON_EPSILON && back >= -ON_EPSILON)) {
			//keep the current start and end point on the stack
			//and go down the tree with the front child
			tstack_p->nodenum = aasnode->children[0];
			tstack_p++;
			if (tstack_p >= &tracestack[127]) {
				botimport.Print(PRT_ERROR, "AAS_TraceBoundingBox: stack overflow\n");
				return trace;
			}
		}
		//if the whole to be traced line is totally at the back of this node
		//only go down the tree with the back child
		else if ((front < ON_EPSILON && back < ON_EPSILON)) {
			//keep the current start and end point on the stack
			//and go down the tree with the back child
			tstack_p->nodenum = aasnode->children[1];
			tstack_p++;
			if (tstack_p >= &tracestack[127]) {
				botimport.Print(PRT_ERROR, "AAS_TraceBoundingBox: stack overflow\n");
				return trace;
			}
		}
		//go down the tree both at the front and back of the node
		else
		{
			tmpplanenum = tstack_p->planenum;
			//
			if (frac < 0) {
				frac = 0.001; //0
			}
			else if (frac > 1) {
				frac = 0.999; //1
			}
			//frac = front / (front-back);
			//
			cur_mid[0] = cur_start[0] + (cur_end[0] - cur_start[0]) * frac;
			cur_mid[1] = cur_start[1] + (cur_end[1] - cur_start[1]) * frac;
			cur_mid[2] = cur_start[2] + (cur_end[2] - cur_start[2]) * frac;

			//			AAS_DrawPlaneCross(cur_mid, plane->normal, plane->dist, plane->type, LINECOLOR_RED);
						//side the front part of the line is on
			side = front < 0;
			//first put the end part of the line on the stack (back side)
			VectorCopy(cur_mid, tstack_p->start);
			//not necesary to store because still on stack
			//VectorCopy(cur_end, tstack_p->end);
			tstack_p->planenum = aasnode->planenum;
			tstack_p->nodenum = aasnode->children[!side];
			tstack_p++;
			if (tstack_p >= &tracestack[127]) {
				botimport.Print(PRT_ERROR, "AAS_TraceBoundingBox: stack overflow\n");
				return trace;
			}
			//now put the part near the start of the line on the stack so we will
			//continue with thats part first. This way we'll find the first
			//hit of the bbox
			VectorCopy(cur_start, tstack_p->start);
			VectorCopy(cur_mid, tstack_p->end);
			tstack_p->planenum = tmpplanenum;
			tstack_p->nodenum = aasnode->children[side];
			tstack_p++;
			if (tstack_p >= &tracestack[127]) {
				botimport.Print(PRT_ERROR, "AAS_TraceBoundingBox: stack overflow\n");
				return trace;
			}
		} 
	} 
//	return trace;
} 

/*
=======================================
AAS_TraceAreas
	- Traces a line from one point to another, recording all the AAS areas the line passes through. This function is used in pathfinding and determining visibility between points.
=======================================
*/
int AAS_TraceAreas(vec3_t start, vec3_t end, int* areas, vec3_t* points, int maxareas) {
	int side, nodenum, tmpplanenum;
	int numareas;
	float front, back, frac;
	vec3_t cur_start, cur_end, cur_mid;
	aas_tracestack_t tracestack[127];
	aas_tracestack_t* tstack_p;
	aas_node_t* aasnode;
	aas_plane_t* plane;

	numareas = 0;
	areas[0] = 0;
	if (!aasworld->loaded) {
		return numareas;
	}

	tstack_p = tracestack;

	//we start with the whole line on the stack
	VectorCopy(start, tstack_p->start);
	VectorCopy(end, tstack_p->end);
	tstack_p->planenum = 0;

	//start with node 1 because node zero is a dummy for a solid leaf
	tstack_p->nodenum = 1;      //starting at the root of the tree
	tstack_p++;

	while (1)
	{
		//pop up the stack
		tstack_p--;
		//if the trace stack is empty (ended up with a piece of the
		//line to be traced in an area)
		if (tstack_p < tracestack) {
			return numareas;
		}
		//number of the current node to test the line against
		nodenum = tstack_p->nodenum;
		//if it is an area
		if (nodenum < 0) {
#ifdef AAS_SAMPLE_DEBUG
			if (-nodenum > aasworld->numareasettings) {
				botimport.Print(PRT_ERROR, "AAS_TraceAreas: -nodenum = %d out of range\n", -nodenum);
				return numareas;
			}
#endif //AAS_SAMPLE_DEBUG
			//botimport.Print(PRT_MESSAGE, "areanum = %d, must be %d\n", -nodenum, AAS_PointAreaNum(start));
			areas[numareas] = -nodenum;
			if (points) {
				VectorCopy(tstack_p->start, points[numareas]);
			}
			numareas++;
			if (numareas >= maxareas) {
				return numareas;
			}
			continue;
		}
		//if it is a solid leaf
		if (!nodenum) {
			continue;
		}
#ifdef AAS_SAMPLE_DEBUG
		if (nodenum > aasworld->numnodes) {
			botimport.Print(PRT_ERROR, "AAS_TraceAreas: nodenum out of range\n");
			return numareas;
		}
#endif //AAS_SAMPLE_DEBUG
		//the node to test against
		aasnode = &aasworld->nodes[nodenum];
		//start point of current line to test against node
		VectorCopy(tstack_p->start, cur_start);
		//end point of the current line to test against node
		VectorCopy(tstack_p->end, cur_end);
		//the current node plane
		plane = &aasworld->planes[aasnode->planenum];

		switch (plane->type)
		{/*FIXME: wtf doesn't this work? obviously the node planes aren't always facing positive!!!
			//check for axial planes
			case PLANE_X:
			{
				front = cur_start[0] - plane->dist;
				back = cur_end[0] - plane->dist;
				break;
			} //end case
			case PLANE_Y:
			{
				front = cur_start[1] - plane->dist;
				back = cur_end[1] - plane->dist;
				break;
			} //end case
			case PLANE_Z:
			{
				front = cur_start[2] - plane->dist;
				back = cur_end[2] - plane->dist;
				break;
			} //end case*/
		default:     //gee it's not an axial plane
		{
			front = DotProduct(cur_start, plane->normal) - plane->dist;
			back = DotProduct(cur_end, plane->normal) - plane->dist;
			break;
		}     //end default
		} //end switch

		//if the whole to be traced line is totally at the front of this node
		//only go down the tree with the front child
		if (front > 0 && back > 0) {
			//keep the current start and end point on the stack
			//and go down the tree with the front child
			tstack_p->nodenum = aasnode->children[0];
			tstack_p++;
			if (tstack_p >= &tracestack[127]) {
				botimport.Print(PRT_ERROR, "AAS_TraceAreas: stack overflow\n");
				return numareas;
			}
		}
		//if the whole to be traced line is totally at the back of this node
		//only go down the tree with the back child
		else if (front <= 0 && back <= 0) {
			//keep the current start and end point on the stack
			//and go down the tree with the back child
			tstack_p->nodenum = aasnode->children[1];
			tstack_p++;
			if (tstack_p >= &tracestack[127]) {
				botimport.Print(PRT_ERROR, "AAS_TraceAreas: stack overflow\n");
				return numareas;
			}
		}
		//go down the tree both at the front and back of the node
		else
		{
			tmpplanenum = tstack_p->planenum;
			//calculate the hitpoint with the node (split point of the line)
			//put the crosspoint TRACEPLANE_EPSILON pixels on the near side
			if (front < 0) {
				frac = (front) / (front - back);
			}
			else { frac = (front) / (front - back); }
			if (frac < 0) {
				frac = 0;
			}
			else if (frac > 1) {
				frac = 1;
			}
			//frac = front / (front-back);
			//
			cur_mid[0] = cur_start[0] + (cur_end[0] - cur_start[0]) * frac;
			cur_mid[1] = cur_start[1] + (cur_end[1] - cur_start[1]) * frac;
			cur_mid[2] = cur_start[2] + (cur_end[2] - cur_start[2]) * frac;

			//			AAS_DrawPlaneCross(cur_mid, plane->normal, plane->dist, plane->type, LINECOLOR_RED);
						//side the front part of the line is on
			side = front < 0;
			//first put the end part of the line on the stack (back side)
			VectorCopy(cur_mid, tstack_p->start);
			//not necesary to store because still on stack
			//VectorCopy(cur_end, tstack_p->end);
			tstack_p->planenum = aasnode->planenum;
			tstack_p->nodenum = aasnode->children[!side];
			tstack_p++;
			if (tstack_p >= &tracestack[127]) {
				botimport.Print(PRT_ERROR, "AAS_TraceAreas: stack overflow\n");
				return numareas;
			}
			//now put the part near the start of the line on the stack so we will
			//continue with thats part first. This way we'll find the first
			//hit of the bbox
			VectorCopy(cur_start, tstack_p->start);
			VectorCopy(cur_mid, tstack_p->end);
			tstack_p->planenum = tmpplanenum;
			tstack_p->nodenum = aasnode->children[side];
			tstack_p++;
			if (tstack_p >= &tracestack[127]) {
				botimport.Print(PRT_ERROR, "AAS_TraceAreas: stack overflow\n");
				return numareas;
			}
		} 
	} 
//	return numareas;
} 

/*
=======================================
AAS_OrthogonalToVectors
=======================================
*/
void AAS_OrthogonalToVectors(const vec3_t v1, const vec3_t v2, vec3_t res) {
	res[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
	res[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
	res[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
}

/**
 * Determines if a point is inside the boundaries of a face.
 *
 * This function iterates over each edge of the face, computing a separation normal
 * orthogonal to both the edge vector and the plane's normal vector. It then checks if the
 * given point lies on the expected side of this separation normal for all edges.
 *
 * @param face The face against which the point's position is to be checked.
 * @param pnormal The normal vector of the plane in which the face lies.
 * @param point The point to be tested for its position relative to the face.
 * @param epsilon A small margin used to handle numerical precision issues.
 * @return Returns qtrue if the point lies within the face boundaries, otherwise qfalse.
 */
qboolean AAS_InsideFace(aas_face_t* face, vec3_t pnormal, vec3_t point, float epsilon) {
	if (!aasworld->loaded) {
		return qfalse; // Return immediately if the AAS world is not loaded.
	}

	for (int i = 0; i < face->numedges; i++) {
		int edgenum = aasworld->edgeindex[face->firstedge + i];
		aas_edge_t* edge = &aasworld->edges[abs(edgenum)];

		// Determine the first vertex of the edge based on the edge number's sign.
		int firstvertex = edgenum < 0;
		vec3_t v0, edgevec, pointvec, sepnormal;

		// Copy the first vertex of the edge to v0.
		VectorCopy(aasworld->vertexes[edge->v[firstvertex]], v0);

		// Calculate the edge vector by subtracting v0 from the second vertex.
		VectorSubtract(aasworld->vertexes[edge->v[!firstvertex]], v0, edgevec);

#ifdef AAS_SAMPLE_DEBUG
		// Debugging code to ensure the winding is counter-clockwise.
		static int lastvertex = 0; // Making lastvertex static to persist its value across function calls.
		if (lastvertex && lastvertex != edge->v[firstvertex]) {
			botimport.Print(PRT_MESSAGE, "winding not counter clockwise\n");
		}
		lastvertex = edge->v[!firstvertex];
#endif // AAS_SAMPLE_DEBUG

		// Calculate the vector from the first vertex of the edge to the point.
		VectorSubtract(point, v0, pointvec);

		// Calculate a vector orthogonal to both the edge vector and the face's plane normal.
		AAS_OrthogonalToVectors(edgevec, pnormal, sepnormal);

		// Check on which side of the plane defined by the orthogonal vector the point is.
		if (DotProduct(pointvec, sepnormal) < -epsilon) {
			return qfalse; // The point is outside the face if on the negative side of the plane.
		}
	}

	return qtrue; // The point is inside the face if it passes all edge checks.
}

/**
 * Checks if a given point is inside a specific face.
 *
 * This function determines whether a point is within the boundaries of a face
 * by checking against all edges of the face. It uses the cross product to find
 * a separation normal and then checks if the point lies on the correct side of
 * each edge.
 *
 * @param facenum The index of the face to check against.
 * @param point The point to test.
 * @param epsilon A small value used to widen the edge boundaries slightly for the test.
 * @return Returns qtrue if the point is inside the face, otherwise qfalse.
 */
qboolean AAS_PointInsideFace(int facenum, vec3_t point, float epsilon) {
	if (!aasworld->loaded) {
		return qfalse;
	}

	aas_face_t* face = &aasworld->faces[facenum];
	aas_plane_t* plane = &aasworld->planes[face->planenum];

	for (int i = 0; i < face->numedges; i++) {
		int edgenum = aasworld->edgeindex[face->firstedge + i];
		aas_edge_t* edge = &aasworld->edges[abs(edgenum)];
		int firstvertex = edgenum < 0;
		vec_t* v1 = aasworld->vertexes[edge->v[firstvertex]];
		vec_t* v2 = aasworld->vertexes[edge->v[!firstvertex]];
		vec3_t edgevec, pointvec, sepnormal;

		VectorSubtract(v2, v1, edgevec);
		VectorSubtract(point, v1, pointvec);
		CrossProduct(edgevec, plane->normal, sepnormal);

		if (DotProduct(pointvec, sepnormal) < -epsilon) {
			return qfalse;
		}
	}
	return qtrue;
}

/**
 * Finds the ground face that a point is above within a specified area.
 *
 * This function iterates through all faces of an area to find a ground face
 * that the given point is above. It uses the normal of the ground faces to
 * determine if the point is considered "above" the face.
 *
 * @param areanum The area number to search for the ground face.
 * @param point The point to use for finding the ground face.
 * @return Pointer to the ground face if found, otherwise NULL.
 */
aas_face_t* AAS_AreaGroundFace(int areanum, vec3_t point) {
	if (!aasworld->loaded) {
		return NULL;
	}

	aas_area_t* area = &aasworld->areas[areanum];
	vec3_t up = { 0, 0, 1 }, normal;

	for (int i = 0; i < area->numfaces; i++) {
		int facenum = aasworld->faceindex[area->firstface + i];
		aas_face_t* face = &aasworld->faces[abs(facenum)];

		if (face->faceflags & FACE_GROUND) {
			VectorCopy((aasworld->planes[face->planenum].normal[2] < 0) ? up : up, normal);
			if (AAS_InsideFace(face, normal, point, 0.01)) {
				return face;
			}
		}
	}
	return NULL;
}

/**
 * Retrieves the plane of a given face.
 *
 * This function gets the plane normal and distance to the origin for the plane
 * associated with a specific face.
 *
 * @param facenum The index of the face.
 * @param normal Output vector for the plane's normal.
 * @param dist Output float for the distance of the plane from the origin.
 */
void AAS_FacePlane(int facenum, vec3_t normal, float* dist) {
	aas_plane_t* plane = &aasworld->planes[aasworld->faces[facenum].planenum];
	VectorCopy(plane->normal, normal);
	*dist = plane->dist;
}

/*
============================================
AAS_TraceEndFace
	-Determines the face where the trace ends within the AAS world.
	-
	-This function finds the face in the last area that the trace went through
	-which is in the same plane as the end point of the trace. If the trace started
	-in solid, it immediately returns NULL since no face can be hit.
	-
	-@param trace The trace result to evaluate.
	-@return Pointer to the aas_face_t structure of the face where the trace ends, or NULL if not applicable.
 ============================================
 */
aas_face_t* AAS_TraceEndFace(aas_trace_t* trace) {
	if (!aasworld->loaded || trace->startsolid) {
		return NULL; // AAS world not loaded or trace started in solid.
	}

	aas_area_t* area = &aasworld->areas[trace->lastarea]; // Last area the trace was in.
	for (int i = 0; i < area->numfaces; i++) {
		int facenum = aasworld->faceindex[area->firstface + i];
		aas_face_t* face = &aasworld->faces[abs(facenum)];
		// Check if the face is in the same plane as the trace end point.
		if ((face->planenum & ~1) == (trace->planenum & ~1)) {
			// Check if the trace end position is inside the face.
			if (AAS_InsideFace(face, aasworld->planes[face->planenum].normal, trace->endpos, 0.01)) {
				return face;
			}
		}
	}
	return NULL; // No matching face found.
}

/*
=================================================
AAS_BoxOnPlaneSide2
  -Determines on which sides of a plane a bounding box lies.
  -
  -This function is used to classify a bounding box relative to a plane, determining
  -whether it is in front of, behind, or spanning the plane.
  -
  -@param absmins The minimum bounds of the box.
  -@param absmaxs The maximum bounds of the box.
  -@param p The plane against which the box is to be tested.
  -@return An integer indicating the box's position relative to the plane:
  -        1 if the box is in front of the plane,
  -        2 if the box is behind the plane,
  -        3 if the box spans the plane.
=================================================
*/
int AAS_BoxOnPlaneSide2(vec3_t absmins, vec3_t absmaxs, aas_plane_t* p) {
	vec3_t corners[2];
	for (int i = 0; i < 3; i++) {
		if (p->normal[i] < 0) {
			corners[0][i] = absmins[i];
			corners[1][i] = absmaxs[i];
		}
		else {
			corners[1][i] = absmins[i];
			corners[0][i] = absmaxs[i];
		}
	}

	float dist1 = DotProduct(p->normal, corners[0]) - p->dist;
	float dist2 = DotProduct(p->normal, corners[1]) - p->dist;
	int sides = 0;
	if (dist1 >= 0) sides = 1;
	if (dist2 < 0) sides |= 2;

	return sides;
}

/**
 * Determines on which side(s) of a plane a bounding box lies.
 *
 * This function checks if a bounding box is in front of, behind, or spanning a given plane.
 * For planes aligned with the world's axes (axial planes), a simplified check is performed.
 * For non-axial planes, AAS_BoxOnPlaneSide2 is used for the determination.
 *
 * @param absmins The minimum bounds of the box.
 * @param absmaxs The maximum bounds of the box.
 * @param p The plane against which the box is to be tested.
 * @return An integer indicating the box's position relative to the plane:
 *         1 if the box is in front of the plane,
 *         2 if the box is behind the plane,
 *         3 if the box spans the plane.
 */
int AAS_BoxOnPlaneSide(vec3_t absmins, vec3_t absmaxs, aas_plane_t* p) {
	if (p->type < 3) {
		// For axial planes, perform a simplified check.
		if (p->dist <= absmins[p->type]) {
			return 1; // Box is completely in front of the plane.
		}
		else if (p->dist >= absmaxs[p->type]) {
			return 2; // Box is completely behind the plane.
		}
		else {
			return 3; // Box spans the plane.
		}
	}
	else {
		// For non-axial planes, use the more complex function.
		return AAS_BoxOnPlaneSide2(absmins, absmaxs, p);
	}
}

/**
 * Removes the links to an entity from all areas it is linked to.
 *
 * This function iterates through all areas an entity is linked to and removes the entity
 * from those areas' linked lists. It also deallocates the used link structures.
 *
 * @param areas A pointer to the linked list of areas the entity is linked to.
 */
void AAS_UnlinkFromAreas(aas_link_t* areas) {
	for (aas_link_t* link = areas, *nextlink; link; link = nextlink) {
		// Save the next link in the area because current link will be deallocated
		nextlink = link->next_area;

		// If there is a previous entity, adjust its next_ent to skip this link
		if (link->prev_ent) {
			link->prev_ent->next_ent = link->next_ent;
		}
		else {
			// If there is no previous entity, this link is the first in the area,
			// so update the area's linked entities to skip this link
			aasworld->arealinkedentities[link->areanum] = link->next_ent;
		}

		// If there is a next entity, adjust its prev_ent to skip this link
		if (link->next_ent) {
			link->next_ent->prev_ent = link->prev_ent;
		}

		// Deallocate the link structure
		AAS_DeAllocAASLink(link);
	}
}

/**
 * Links an entity to all AAS areas that its bounding box is totally or partly situated in.
 *
 * This function uses recursion down the BSP tree, using the bounding box to test for plane sides,
 * and links the entity to the areas it intersects.
 *
 * @param absmins The absolute minimum bounds of the entity's bounding box.
 * @param absmaxs The absolute maximum bounds of the entity's bounding box.
 * @param entnum The entity number to link.
 * @return A pointer to the first link in a linked list of aas_link_t structures representing the areas the entity is linked to.
 */
aas_link_t* AAS_AASLinkEntity(vec3_t absmins, vec3_t absmaxs, int entnum) {
	if (!aasworld->loaded) {
		botimport.Print(PRT_ERROR, "AAS_LinkEntity: AAS not loaded\n");
		return NULL;
	}

	aas_linkstack_t linkstack[128], * lstack_p = linkstack;
	aas_link_t* link, * areas = NULL;

	// Start with the root of the BSP tree
	lstack_p->nodenum = 1;
	lstack_p++;

	while (lstack_p > linkstack) {
		// Pop up the stack
		lstack_p--;
		int nodenum = lstack_p->nodenum;

		// If it is an area
		if (nodenum < 0) {
			// Prevent linking the entity multiple times to the same area
			for (link = aasworld->arealinkedentities[-nodenum]; link; link = link->next_ent) {
				if (link->entnum == entnum) break;
			}
			if (link) continue;

			link = AAS_AllocAASLink();
			if (!link) return areas; // Allocation failed

			// Set up the new link
			link->entnum = entnum;
			link->areanum = -nodenum;
			// Insert at the beginning of the areas list
			link->next_area = areas;
			if (areas) areas->prev_area = link;
			areas = link;
			// Insert at the beginning of the area's entity list
			link->next_ent = aasworld->arealinkedentities[-nodenum];
			if (aasworld->arealinkedentities[-nodenum]) aasworld->arealinkedentities[-nodenum]->prev_ent = link;
			aasworld->arealinkedentities[-nodenum] = link;
			continue;
		}

		if (!nodenum) continue; // If solid leaf, skip

		// Determine which side(s) of the node the box is on
		aas_node_t* aasnode = &aasworld->nodes[nodenum];
		aas_plane_t* plane = &aasworld->planes[aasnode->planenum];
		int side = AAS_BoxOnPlaneSide2(absmins, absmaxs, plane);

		// Recurse down the tree
		if (side & 1) {
			lstack_p->nodenum = aasnode->children[0];
			lstack_p++;
		}
		if (side & 2) {
			lstack_p->nodenum = aasnode->children[1];
			lstack_p++;
		}

		// Check for stack overflow
		if (lstack_p >= &linkstack[127]) {
			botimport.Print(PRT_ERROR, "AAS_LinkEntity: stack overflow\n");
			break;
		}
	}

	return areas;
}

/**
 * Relinks an entity in the AAS world with an adjusted bounding box based on the specified presence type.
 *
 * @param absmins The absolute minimum bounds of the entity's bounding box.
 * @param absmaxs The absolute maximum bounds of the entity's bounding box.
 * @param entnum The entity number to relink.
 * @param presencetype The presence type to use for adjusting the bounding box.
 * @return A pointer to a linked list of aas_link_t structures representing the areas the entity is linked to.
 */
aas_link_t* AAS_LinkEntityClientBBox(vec3_t absmins, vec3_t absmaxs, int entnum, int presencetype) {
	vec3_t mins, maxs;
	vec3_t newabsmins, newabsmaxs;

	// Calculate the adjusted bounding box based on the presence type.
	AAS_PresenceTypeBoundingBox(presencetype, mins, maxs);
	VectorSubtract(absmins, maxs, newabsmins);
	VectorSubtract(absmaxs, mins, newabsmaxs);

	// Relink the entity with the adjusted bounding box in the AAS world.
	return AAS_AASLinkEntity(newabsmins, newabsmaxs, entnum);
}

/**
 * Retrieves a plane from the AAS world using a plane number.
 *
 * @param planenum The index of the plane in the AAS world's planes array.
 * @return A pointer to the aas_plane_t structure if the AAS world is loaded; otherwise, NULL.
 */
aas_plane_t* AAS_PlaneFromNum(int planenum) {
	if (!aasworld->loaded) {
		return NULL; // AAS world not loaded.
	}

	return &aasworld->planes[planenum];
}

/**
 * Finds all areas that are touched by a specified bounding box.
 *
 * @param absmins The absolute minimum bounds of the bounding box.
 * @param absmaxs The absolute maximum bounds of the bounding box.
 * @param areas An array to store the area numbers of touched areas.
 * @param maxareas The maximum number of area numbers that can be stored in the areas array.
 * @return The number of areas found and stored in the areas array.
 */
int AAS_BBoxAreas(vec3_t absmins, vec3_t absmaxs, int* areas, int maxareas) {
	aas_link_t* linkedareas = AAS_AASLinkEntity(absmins, absmaxs, -1);
	int num = 0;

	// Iterate through the linked areas and store their numbers in the areas array.
	for (aas_link_t* link = linkedareas; link && num < maxareas; link = link->next_area) {
		areas[num++] = link->areanum;
	}

	// Unlink the areas from the temporary linked list.
	AAS_UnlinkFromAreas(linkedareas);

	return num;
}
