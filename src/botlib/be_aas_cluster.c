// be_aas_cluster.c
//

/*
==============================================
AAS Cluster Code

A cluster represents a collection of interconnected spatial areas that are grouped based on their reachability without the need to traverse through a portal. 
Essentially, a cluster forms a localized network within the larger game map, where each area within the cluster can be reached from any other area in the 
same cluster directly or through a series of connections within that cluster.

A cluster portal acts as a gateway or transitional area that connects distinct clusters—groups of spatially and logically connected areas within a game map 
that are internally navigable without passing through a portal. These portals are strategically placed at the junctures between clusters, enabling AI entities to 
traverse from one cluster to another, thereby navigating the broader game environment beyond their immediate vicinity. Cluster portals are instrumental in optimizing pathfinding algorithms, 
as they allow for the segmentation of the game map into manageable sections, reducing the computational overhead required for AI to calculate paths. 
 
Example 1:
Below is an ASCII diagram depicting the concept of AAS clusters and portals in a simplified game map layout. This diagram illustrates three clusters (A, B, C) interconnected by portals (X, Y).

+---------+      X     +---------+      Y     +---------+
|         |------------|         |------------|         |
| Cluster |            | Cluster |            | Cluster |
|    A    |            |    B    |            |    C    |
|         |            |         |            |         |
+---------+            +---------+            +---------+
	 |                      |                      |
	 |                      |                      |
	 +--- Areas connected   +--- Areas connected   +--- Areas connected
		 within Cluster A       within Cluster B       within Cluster C

Clusters (A, B, C): These represent groups of areas within the game map that are directly reachable from one another without needing to go through a portal. 
For example, all areas within Cluster A can be navigated freely within A itself.

Portals (X, Y): These are special areas that serve as gateways between clusters. To move from one cluster to another (e.g., from Cluster A to Cluster B), an entity must pass through a portal (in this case, Portal X).

Example 2:
Room 1 Cluster        Portal        Room 2 Cluster        Portal        Room 3 Cluster
+-----------+                       +-----------+                       +-----------+
|           |           |           |           |           |           |           |
|     |     |-----------|-----------|     |     |-----------|-----------|     |     |
|     P     |           |           |     P     |           |           |     P     |
|           |           |           |           |           |           |           |
+-----------+                       +-----------+                       +-----------+
	  |                                  |                                  |
	  |                                  |                                  |
	  +---- Areas around the Pillar     +---- Areas around the Pillar     +---- Areas around the Pillar
			in Room 1 Cluster                 in Room 2 Cluster                 in Room 3 Cluster

Rooms (1, 2, 3 Clusters): Each room is considered its own cluster due to the navigable area around the central pillar (P). 
						  Entities within the room can move freely around the pillar but cannot pass directly through it, defining the room's cluster boundaries.

Pillar (P): The pillar in the middle of each room acts as a natural obstacle, around which the navigable space (and thus the cluster) is defined.

Portals: The doorways connecting each room are considered portals. These are the transitions points through which an entity moves from one cluster to another.
==============================================
*/

#include "../game/q_shared.h"
#include "../botutil/l_memory.h"
#include "../botutil/l_script.h"
#include "../botutil/l_precomp.h"
#include "../botutil/l_struct.h"
#include "../botutil/l_log.h"
#include "../botutil/l_memory.h"
#include "../botutil/l_libvar.h"
#include "aasfile.h"
#include "../game/botlib.h"
#include "../game/be_aas.h"
#include "be_aas_funcs.h"
#include "be_aas_def.h"

extern botlib_import_t botimport;

#define AAS_MAX_PORTALS                 65536
#define AAS_MAX_PORTALINDEXSIZE         65536
#define AAS_MAX_CLUSTERS                65536
//
#define MAX_PORTALAREAS         1024

// do not flood through area faces, only use reachabilities
int nofaceflood = qtrue;

/*
==============================
AS_RemoveClusterAreas

Resets the cluster index for each area in the global aasworld structure to 0. This function is used to clear existing cluster data.
==============================
*/
void AAS_RemoveClusterAreas( void ) {
	int i;

	for ( i = 1; i < aasworld->numareas; i++ )
	{
		aasworld->areasettings[i].cluster = 0;
	}
}

/*
==========================================
AAS_ClearCluster
	- Sets the cluster index to 0 for all areas belonging to the specified cluster. It's a more targeted version of AAS_RemoveClusterAreas, affecting only areas within a given cluster.
Parameters: 
	clusternum (int) - The cluster number to clear.
==========================================
*/
void AAS_ClearCluster( int clusternum ) {
	int i;

	for ( i = 1; i < aasworld->numareas; i++ )
	{
		if ( aasworld->areasettings[i].cluster == clusternum ) {
			aasworld->areasettings[i].cluster = 0;
		} 
	} 
}

/*
=====================================
AAS_RemovePortalsClusterReference
	- Clears the cluster references in portals that lead to or from the specified cluster. This is used when modifying the cluster structure, ensuring portals don't reference non-existent or outdated clusters.
Parameters: 
	clusternum (int) - The cluster number whose references are to be removed from portals.
=====================================
*/
void AAS_RemovePortalsClusterReference( int clusternum ) {
	int portalnum;

	for ( portalnum = 1; portalnum < aasworld->numportals; portalnum++ )
	{
		if ( aasworld->portals[portalnum].frontcluster == clusternum ) {
			aasworld->portals[portalnum].frontcluster = 0;
		}
		if ( aasworld->portals[portalnum].backcluster == clusternum ) {
			aasworld->portals[portalnum].backcluster = 0;
		}
	}
}

/*
=====================================
AAS_UpdatePortal
   Updates the portal information for a specified area and cluster.
   
   Parameters:
   - areanum: The area number to update the portal information for.
   - clusternum: The cluster number to associate with the area.
   
   Returns:
   - qtrue if the update was successful, qfalse otherwise.
=====================================
*/
int AAS_UpdatePortal(int areanum, int clusternum) {
	int portalnum;
	aas_portal_t* portal;
	aas_cluster_t* cluster;

	// Search for the portal corresponding to the specified area.
	for (portalnum = 1; portalnum < aasworld->numportals; portalnum++) {
		if (aasworld->portals[portalnum].areanum == areanum) {
			break; // Found the matching portal.
		}
	}

	// If no portal is found for the area, report an error and return true.
	if (portalnum == aasworld->numportals) {
		AAS_Error("No portal found for area %d", areanum);
		return qtrue; // Using qtrue to indicate non-success as per original code's convention.
	}

	portal = &aasworld->portals[portalnum];

	// Check if the portal is already associated with the specified cluster.
	if (portal->frontcluster == clusternum || portal->backcluster == clusternum) {
		return qtrue; // Portal is already updated, no further action required.
	}

	// Assign the cluster to the portal's front or back cluster, if they are not set.
	if (!portal->frontcluster) {
		portal->frontcluster = clusternum;
	}
	else if (!portal->backcluster) {
		portal->backcluster = clusternum;
	}
	else {
		// The portal separates more than two clusters, which is not supported.
		Log_Write("Portal using area %d separates more than two clusters\r\n", areanum);

		// Clear the cluster portal flag for the area to indicate an error state.
		aasworld->areasettings[areanum].contents &= ~AREACONTENTS_CLUSTERPORTAL;

		return qfalse; // Return false to indicate failure.
	}

	// Ensure we do not exceed the maximum allowed portal index size.
	if (aasworld->portalindexsize >= AAS_MAX_PORTALINDEXSIZE) {
		AAS_Error("Exceeded AAS_MAX_PORTALINDEXSIZE");
		return qtrue; // Using qtrue for consistency, indicating a limit issue.
	}

	// Set the area's cluster number to the negative portal number to indicate it's a portal.
	aasworld->areasettings[areanum].cluster = -portalnum;

	// Add the portal to the cluster's portal index and update counts.
	cluster = &aasworld->clusters[clusternum];
	aasworld->portalindex[cluster->firstportal + cluster->numportals] = portalnum;
	aasworld->portalindexsize++;
	cluster->numportals++;

	return qtrue; // Indicate success.
}

/*
=====================================
AAS_FloodClusterAreas_r
   Recursively assigns areas to a cluster.
  
   Parameters:
   - areanum: The area number to start the flood fill from.
   - clusternum: The cluster number to assign to the areas.
  
   Returns:
   - qtrue if successful, qfalse if an error occurs.
=====================================
*/
int AAS_FloodClusterAreas_r(int areanum, int clusternum) {
	// Validate area number.
	if (areanum <= 0 || areanum >= aasworld->numareas) {
		AAS_Error("AAS_FloodClusterAreas_r: areanum out of range");
		return qfalse;
	}

	// Skip areas already part of a cluster, unless it's the current one.
	if (aasworld->areasettings[areanum].cluster > 0) {
		if (aasworld->areasettings[areanum].cluster == clusternum) {
			return qtrue;
		}
		AAS_Error("Cluster %d touched cluster %d at area %d",
			clusternum, aasworld->areasettings[areanum].cluster, areanum);
		return qfalse;
	}

	// Skip cluster portal areas, updating portal info instead.
	if (aasworld->areasettings[areanum].contents & AREACONTENTS_CLUSTERPORTAL) {
		return AAS_UpdatePortal(areanum, clusternum);
	}

	// Assign the area to the cluster.
	aasworld->areasettings[areanum].cluster = clusternum;
	aasworld->areasettings[areanum].clusterareanum = aasworld->clusters[clusternum].numareas;
	aasworld->clusters[clusternum].numareas++;

	// Flood into adjacent areas using area faces.
	aas_area_t* area = &aasworld->areas[areanum];
	if (!nofaceflood) {
		for (int i = 0; i < area->numfaces; i++) {
			int facenum = abs(aasworld->faceindex[area->firstface + i]);
			aas_face_t* face = &aasworld->faces[facenum];
			int nextArea = (face->frontarea == areanum) ? face->backarea : face->frontarea;
			
			if (nextArea && !AAS_FloodClusterAreas_r(nextArea, clusternum)) {
				return qfalse;
			}
		}
	}

	// Flood into areas reachable through the area's reachabilities.
	for (int i = 0; i < aasworld->areasettings[areanum].numreachableareas; i++) {
		int nextArea = aasworld->reachability[aasworld->areasettings[areanum].firstreachablearea + i].areanum;

		if (nextArea && !AAS_FloodClusterAreas_r(nextArea, clusternum)) {
			return qfalse;
		}
	}

	return qtrue;
}
/*
=====================================
AAS_FloodClusterAreasUsingReachabilities
  Tries to assign areas without a cluster to the specified cluster using reachabilities.
 
  Parameters:
  - clusternum: The cluster number to assign to the areas.
 
  Returns:
  - qtrue if successful, qfalse otherwise.
=====================================
*/
int AAS_FloodClusterAreasUsingReachabilities(int clusternum) {
    for (int i = 1; i < aasworld->numareas; i++) {
        // Skip areas already in a cluster or marked as cluster portals.
        if (aasworld->areasettings[i].cluster ||
            (aasworld->areasettings[i].contents & AREACONTENTS_CLUSTERPORTAL)) {
            continue;
        }

        // Check reachabilities to find areas that can join this cluster.
        for (int j = 0; j < aasworld->areasettings[i].numreachableareas; j++) {
            int areanum = aasworld->reachability[aasworld->areasettings[i].firstreachablearea + j].areanum;
            
			// Skip cluster portals and areas already in a cluster.
            if ((aasworld->areasettings[areanum].contents & AREACONTENTS_CLUSTERPORTAL) ||
                aasworld->areasettings[areanum].cluster) {
                
				// Try to flood the area with the current cluster.
                if (!AAS_FloodClusterAreas_r(i, clusternum)) {
                    return qfalse;
                }
                
				// Restart the search as the cluster assignment could change the setup.
                i = 0;
                break;
            }
        }
    }

    return qtrue;
}

/*
============================================
AAS_NumberClusterPortals
  Numbers the portals of a given cluster.
 
  Parameters:
  - clusternum: The cluster number whose portals are to be numbered.
============================================
*/
void AAS_NumberClusterPortals( int clusternum ) {
	aas_cluster_t* cluster = &aasworld->clusters[clusternum];

	// Assign a number to each portal in the cluster.
	for (int i = 0; i < cluster->numportals; i++) {
		int portalnum = aasworld->portalindex[cluster->firstportal + i];
		aas_portal_t* portal = &aasworld->portals[portalnum];

		// Determine which side of the portal faces the cluster and assign the area number.
		if (portal->frontcluster == clusternum) {
			portal->clusterareanum[0] = cluster->numareas++;
		}
		else {
			portal->clusterareanum[1] = cluster->numareas++;
		}
	}
}

/*
==========================================
AAS_NumberClusterAreas
 Numbers areas and portals within a specified cluster, differentiating between those with and without reachabilities.

 Parameters:
 - clusternum: The cluster number to process.
==========================================
*/
void AAS_NumberClusterAreas(int clusternum) {
	int i, portalnum;
	aas_cluster_t* cluster;
	aas_portal_t* portal;

	// Reset the number of areas and reachability areas in the cluster to 0.
	aasworld->clusters[clusternum].numareas = 0;
	aasworld->clusters[clusternum].numreachabilityareas = 0;

	// First pass: Number all areas in this cluster that have reachabilities.
	for (i = 1; i < aasworld->numareas; i++) {
		if (aasworld->areasettings[i].cluster == clusternum && AAS_AreaReachability(i)) {
			aasworld->areasettings[i].clusterareanum = aasworld->clusters[clusternum].numareas;
			aasworld->clusters[clusternum].numareas++;
			aasworld->clusters[clusternum].numreachabilityareas++;
		}
	}

	// Number all portals in this cluster that have reachabilities.
	cluster = &aasworld->clusters[clusternum];
	for (i = 0; i < cluster->numportals; i++) {
		portalnum = aasworld->portalindex[cluster->firstportal + i];
		portal = &aasworld->portals[portalnum];
		if (AAS_AreaReachability(portal->areanum)) {
			// Assign number based on which cluster the portal faces.
			int clusterSideIndex = (portal->frontcluster == clusternum) ? 0 : 1;
			portal->clusterareanum[clusterSideIndex] = cluster->numareas++;
			aasworld->clusters[clusternum].numreachabilityareas++;
		}
	}

	// Second pass: Number all areas in this cluster without reachabilities.
	for (i = 1; i < aasworld->numareas; i++) {
		if (aasworld->areasettings[i].cluster == clusternum && !AAS_AreaReachability(i)) {
			aasworld->areasettings[i].clusterareanum = aasworld->clusters[clusternum].numareas;
			aasworld->clusters[clusternum].numareas++;
		}
	}

	// Number all portals in this cluster without reachabilities.
	for (i = 0; i < cluster->numportals; i++) {
		portalnum = aasworld->portalindex[cluster->firstportal + i];
		portal = &aasworld->portals[portalnum];
		if (!AAS_AreaReachability(portal->areanum)) {
			int clusterSideIndex = (portal->frontcluster == clusternum) ? 0 : 1;
			portal->clusterareanum[clusterSideIndex] = cluster->numareas++;
		}
	}
}

/*
=========================================
AAS_FindClusters
 Identifies and creates clusters from the game areas.

 Returns:
 - qtrue if successful, qfalse otherwise.
=========================================
*/
int AAS_FindClusters(void) {
	int i;
	aas_cluster_t* cluster;

	// Initializes or clears existing cluster information.
	AAS_RemoveClusterAreas();

	// Iterate through all areas to find and form clusters.
	for (i = 1; i < aasworld->numareas; i++) {
		// Skip areas already assigned to a cluster or marked as cluster portals.
		if (aasworld->areasettings[i].cluster ||
			(aasworld->areasettings[i].contents & AREACONTENTS_CLUSTERPORTAL)) {
			continue;
		}

		// Skip areas without reachabilities if not flooding through faces.
		if (nofaceflood && !aasworld->areasettings[i].numreachableareas) {
			continue;
		}

		// Check if we've exceeded the maximum number of clusters.
		if (aasworld->numclusters >= AAS_MAX_CLUSTERS) {
			AAS_Error("Exceeded AAS_MAX_CLUSTERS limit");
			return qfalse;
		}

		// Initialize the new cluster.
		cluster = &aasworld->clusters[aasworld->numclusters];
		cluster->numareas = cluster->numreachabilityareas = 0;
		cluster->firstportal = aasworld->portalindexsize;
		cluster->numportals = 0;

		// Flood fill the cluster with areas.
		if (!AAS_FloodClusterAreas_r(i, aasworld->numclusters) ||
			!AAS_FloodClusterAreasUsingReachabilities(aasworld->numclusters)) {
			return qfalse;
		}

		// Number the areas within the cluster.
		AAS_NumberClusterAreas(aasworld->numclusters);

		// Increment the cluster count.
		aasworld->numclusters++;
	}

	return qtrue;
}

/*
==========================
AAS_CreatePortals
   -Creates portal structures for areas designated as cluster portals.
==========================
*/
void AAS_CreatePortals(void) {
	int i;
	aas_portal_t* portal;

	// Iterate through all areas to find and initialize portals.
	for (i = 1; i < aasworld->numareas; i++) {
		// Check if the area is marked as a cluster portal.
		if (aasworld->areasettings[i].contents & AREACONTENTS_CLUSTERPORTAL) {
			// Ensure we do not exceed the maximum number of portals.
			if (aasworld->numportals >= AAS_MAX_PORTALS) {
				AAS_Error("Exceeded AAS_MAX_PORTALS limit");
				return;
			}

			// Initialize the portal with area information.
			portal = &aasworld->portals[aasworld->numportals];
			portal->areanum = i;
			portal->frontcluster = portal->backcluster = 0;

			// Log the portal creation for debugging.
			Log_Write("Created portal %d: area %d", aasworld->numportals, portal->areanum);

			// Increment the portal count.
			aasworld->numportals++;
		}
	}
}

/*
==================================================
AAS_ConnectedAreas_r
  Recursively checks if a set of areas are connected.

  Parameters:
  - areanums: Array of area numbers to check for connectivity.
  - numareas: Number of areas in the areanums array.
  - connectedareas: Array tracking which areas have been confirmed as connected.
  - curarea: Current area index in the areanums array being processed.
==================================================
*/
void AAS_ConnectedAreas_r(int* areanums, int numareas, int* connectedareas, int curarea) {
	int i, j, otherareanum, facenum;
	aas_area_t* area;
	aas_face_t* face;

	// Mark the current area as connected.
	connectedareas[curarea] = qtrue;
	area = &aasworld->areas[areanums[curarea]];

	// Iterate through each face of the current area.
	for (i = 0; i < area->numfaces; i++) {
		facenum = abs(aasworld->faceindex[area->firstface + i]);
		face = &aasworld->faces[facenum];

		// Skip solid faces as they do not lead to another area.
		if (face->faceflags & FACE_SOLID) {
			continue;
		}

		// Determine the area on the other side of the face.
		otherareanum = (face->frontarea != areanums[curarea]) ? face->frontarea : face->backarea;

		// Check if the other area is one of the target areas.
		for (j = 0; j < numareas; j++) {
			if (areanums[j] == otherareanum) {
				break;
			}
		}

		// Skip if the other area is not a target or already connected.
		if (j == numareas || connectedareas[j]) {
			continue;
		}

		// Recurse into the connected area.
		AAS_ConnectedAreas_r(areanums, numareas, connectedareas, j);
	}
}

/*
==============================
AAS_ConnectedAreas
 Determines if all specified areas are interconnected.
 
 Parameters:
 - areanums: Array of area numbers to check.
 - numareas: Number of areas in the array.
 
 Returns:
 - qtrue if all areas are interconnected, qfalse otherwise.
 ==============================
 */
qboolean AAS_ConnectedAreas(int* areanums, int numareas) {
	int connectedareas[MAX_PORTALAREAS], i;

	// Initialize connectivity tracking array.
	memset(connectedareas, 0, sizeof(connectedareas));

	// Return false if invalid number of areas.
	if (numareas < 1) {
		return qfalse;
	}

	// All areas are considered connected if there's only one.
	if (numareas == 1) {
		return qtrue;
	}

	// Start recursive connectivity check from the first area.
	AAS_ConnectedAreas_r(areanums, numareas, connectedareas, 0);

	// Verify if all areas are connected.
	for (i = 0; i < numareas; i++) {
		if (!connectedareas[i]) {
			return qfalse;
		}
	}

	return qtrue;
}

/*
===========================================================================
AAS_GetAdjacentAreasWithLessPresenceTypes_r
 Recursively identifies adjacent areas with fewer presence types, aiding in pathfinding or mapping systems by highlighting less restricted navigational paths.

 Parameters:
 - int* areanums: An array to store the area numbers that have been identified.
 - int numareas: The count of areas already added to the areanums array.
 - int curareanum: The current area number being examined.

 Returns:
 - int: The total count of identified areas meeting the criteria, including through recursion.

 Note: This function modifies global variables related to the game world's area properties.
===========================================================================
*/
int AAS_GetAdjacentAreasWithLessPresenceTypes_r(int* areanums, int numareas, int curareanum) {
	int i, j, presencetype, otherpresencetype, otherareanum, facenum;
	aas_area_t* area;
	aas_face_t* face;

	// Add the current area number to the list
	areanums[numareas++] = curareanum;

	// Retrieve the current area and its presence type
	area = &aasworld->areas[curareanum];
	presencetype = aasworld->areasettings[curareanum].presencetype;

	// Iterate through each face of the current area
	for (i = 0; i < area->numfaces; i++) {
		facenum = abs(aasworld->faceindex[area->firstface + i]);
		face = &aasworld->faces[facenum];

		// Skip if the face is solid (not passable)
		if (face->faceflags & FACE_SOLID) {
			continue;
		}

		// Determine the area on the other side of the face
		otherareanum = (face->frontarea != curareanum) ? face->frontarea : face->backarea;

		// Get the presence type of the adjacent area
		otherpresencetype = aasworld->areasettings[otherareanum].presencetype;

		// Check if the other area has fewer presence types and is not already listed
		if ((presencetype & ~otherpresencetype) && !(otherpresencetype & ~presencetype)) {
			for (j = 0; j < numareas; j++) {
				if (otherareanum == areanums[j]) {
					break; // Already listed, no need to add again
				}
			}

			// If not already listed, add the area to the list (if within limits)
			if (j == numareas) {
				if (numareas >= MAX_PORTALAREAS) {
					AAS_Error("MAX_PORTALAREAS");
					return numareas; // Reached maximum capacity
				}
				numareas = AAS_GetAdjacentAreasWithLessPresenceTypes_r(areanums, numareas, otherareanum);
			}
		}
	}

	return numareas; // Return the total count of areas identified
}
/*
===========================================================================
AAS_CheckAreaForPossiblePortals
 Evaluates an area to determine if it can serve as a portal for AI pathfinding.
 Portals help in transitioning between different clusters of navigable space,
 optimizing pathfinding calculations. The function checks if an area meets
 the criteria to be considered a portal, based on its connectivity and the
 characteristics of its neighboring areas.

 Parameters:
 - int areanum: The number of the area being evaluated.

 Returns:
 - int: The number of areas identified as potential portals. Returns 0 if the
        area does not meet the criteria to be considered a portal.

 Changes Globals:
 - Modifies the contents flags of areas in the aasworld global structure to
   mark identified areas as cluster portals and route portals.
===========================================================================
*/
int AAS_CheckAreaForPossiblePortals( int areanum ) {
	int i, j, k, fen, ben, frontedgenum, backedgenum, facenum;
	int areanums[MAX_PORTALAREAS], numareas, otherareanum;
	int numareafrontfaces[MAX_PORTALAREAS], numareabackfaces[MAX_PORTALAREAS];
	int frontfacenums[MAX_PORTALAREAS], backfacenums[MAX_PORTALAREAS];
	int numfrontfaces, numbackfaces;
	int frontareanums[MAX_PORTALAREAS], backareanums[MAX_PORTALAREAS];
	int numfrontareas, numbackareas;
	int frontplanenum, backplanenum, faceplanenum;
	aas_area_t *area;
	aas_face_t *frontface, *backface, *face;

	//if it isn't already a portal
	if ( aasworld->areasettings[areanum].contents & AREACONTENTS_CLUSTERPORTAL ) {
		return 0;
	}

	//it must be a grounded area
	if ( !( aasworld->areasettings[areanum].areaflags & AREA_GROUNDED ) ) {
		return 0;
	}

	memset( numareafrontfaces, 0, sizeof( numareafrontfaces ) );
	memset( numareabackfaces, 0, sizeof( numareabackfaces ) );
	numareas = numfrontfaces = numbackfaces = 0;
	numfrontareas = numbackareas = 0;
	frontplanenum = backplanenum = -1;

	//add any adjacent areas with less presence types
	numareas = AAS_GetAdjacentAreasWithLessPresenceTypes_r( areanums, 0, areanum );

	for ( i = 0; i < numareas; i++ )
	{
		area = &aasworld->areas[areanums[i]];
		for ( j = 0; j < area->numfaces; j++ )
		{
			facenum = abs( aasworld->faceindex[area->firstface + j] );
			face = &aasworld->faces[facenum];
			
			//if the face is solid
			if ( face->faceflags & FACE_SOLID ) {
				continue;
			}
			
			//check if the face is shared with one of the other areas
			for ( k = 0; k < numareas; k++ )
			{
				if ( k == i ) {
					continue;
				}
				if ( face->frontarea == areanums[k] || face->backarea == areanums[k] ) {
					break;
				}
			}
			
			if ( k != numareas ) {
				continue;
			}
			
			//the number of the area at the other side of the face
			if ( face->frontarea == areanums[i] ) {
				otherareanum = face->backarea;
			} else { otherareanum = face->frontarea;}
			
			//if the other area already is a cluter portal
			if ( aasworld->areasettings[otherareanum].contents & AREACONTENTS_CLUSTERPORTAL ) {
				return 0;
			}
			
			//number of the plane of the area
			faceplanenum = face->planenum & ~1;

			if ( frontplanenum < 0 || faceplanenum == frontplanenum ) {
				frontplanenum = faceplanenum;
				frontfacenums[numfrontfaces++] = facenum;
				for ( k = 0; k < numfrontareas; k++ )
				{
					if ( frontareanums[k] == otherareanum ) {
						break;
					}
				}

				if ( k == numfrontareas ) {
					frontareanums[numfrontareas++] = otherareanum;
				}
				numareafrontfaces[i]++;
			}
			else if ( backplanenum < 0 || faceplanenum == backplanenum ) {
				backplanenum = faceplanenum;
				backfacenums[numbackfaces++] = facenum;
				for ( k = 0; k < numbackareas; k++ )
				{
					if ( backareanums[k] == otherareanum ) {
						break;
					}
				}

				if ( k == numbackareas ) {
					backareanums[numbackareas++] = otherareanum;
				}
				numareabackfaces[i]++;
			}
			else
			{
				return 0;
			}
		}
	}
	
	//every area should have at least one front face and one back face
	for ( i = 0; i < numareas; i++ )
	{
		if ( !numareafrontfaces[i] || !numareabackfaces[i] ) {
			return 0;
		}
	}
	
	//the front areas should all be connected
	if ( !AAS_ConnectedAreas( frontareanums, numfrontareas ) ) {
		return 0;
	}
	
	//the back areas should all be connected
	if ( !AAS_ConnectedAreas( backareanums, numbackareas ) ) {
		return 0;
	}

	//none of the front faces should have a shared edge with a back face
	for ( i = 0; i < numfrontfaces; i++ )
	{
		frontface = &aasworld->faces[frontfacenums[i]];
		for ( fen = 0; fen < frontface->numedges; fen++ )
		{
			frontedgenum = abs( aasworld->edgeindex[frontface->firstedge + fen] );
			for ( j = 0; j < numbackfaces; j++ )
			{
				backface = &aasworld->faces[backfacenums[j]];
				for ( ben = 0; ben < backface->numedges; ben++ )
				{
					backedgenum = abs( aasworld->edgeindex[backface->firstedge + ben] );
					if ( frontedgenum == backedgenum ) {
						break;
					}
				}
				if ( ben != backface->numedges ) {
					break;
				}
			}
			if ( j != numbackfaces ) {
				break;
			}
		}
		if ( fen != frontface->numedges ) {
			break;
		}
	}

	if ( i != numfrontfaces ) {
		return 0;
	}

	//set the cluster portal contents
	for ( i = 0; i < numareas; i++ )
	{
		aasworld->areasettings[areanums[i]].contents |= AREACONTENTS_CLUSTERPORTAL;
		//this area can be used as a route portal
		aasworld->areasettings[areanums[i]].contents |= AREACONTENTS_ROUTEPORTAL;
		Log_Write( "possible portal: %d\r\n", areanums[i] );
	}
	return numareas;
}
/*
===========================================================================
AAS_FindPossiblePortals
  Scans all areas in the game world to identify and count possible portals.
  A portal is a transition point between areas, crucial for navigation and pathfinding.

  Parameters:
  - None.

  Returns:
  - Void. The function does not return a value but outputs the count of possible portals to the console.

  Global Changes:
  - This function may internally update structures or counts related to possible portals but does not directly modify global variables.
===========================================================================
*/
void AAS_FindPossiblePortals(void) {
	int i, numpossibleportals;

	// Initialize the count of possible portals to 0
	numpossibleportals = 0;

	// Loop through all areas in the game world
	for (i = 1; i < aasworld->numareas; i++) {
		// Increment the count based on the presence of possible portals in each area
		numpossibleportals += AAS_CheckAreaForPossiblePortals(i);
	}

	// Print the total number of possible portals found
	botimport.Print(PRT_MESSAGE, "\r%6d possible portals\n", numpossibleportals);
}

/*
===========================================================================
AAS_RemoveAllPortals
  Removes all portal markers from all areas in the game world. This can be
  used for resetting or recalculating area connections, such as after dynamic
  level changes or during initialization.

  Parameters:
  - None.

  Returns:
  - Void. The function performs an operation on the game world's data structures but does not return a value.

  Global Changes:
  - Modifies the `areasettings` for each area to clear the `AREACONTENTS_CLUSTERPORTAL` flag, indicating the removal of portal markers.
===========================================================================
*/
void AAS_RemoveAllPortals(void) {
	int i;

	// Loop through all areas in the game world
	for (i = 1; i < aasworld->numareas; i++) {
		// Clear the portal marker flag from each area
		aasworld->areasettings[i].contents &= ~AREACONTENTS_CLUSTERPORTAL;
	}
}

/*
===========================================================================
AAS_TestPortals

 Tests all portals to ensure they have valid front and back clusters. Invalid
 portals are flagged by removing the AREACONTENTS_CLUSTERPORTAL content flag
 and logged for further investigation.

 Parameters: None.

 Returns:
 - int: Returns qtrue if all portals are valid, qfalse otherwise.

 Changes Globals:
 - Modifies areasettings of aasworld to remove AREACONTENTS_CLUSTERPORTAL from
   invalid portals.
===========================================================================
*/
int AAS_TestPortals(void) {
	int i;
	aas_portal_t* portal;

	for (i = 1; i < aasworld->numportals; i++) {
		portal = &aasworld->portals[i];
		if (!portal->frontcluster || !portal->backcluster) {
			aasworld->areasettings[portal->areanum].contents &= ~AREACONTENTS_CLUSTERPORTAL;
			Log_Write("portal area %d has no front/back cluster\n", portal->areanum);
			return qfalse;
		}
	}
	return qtrue;
}

/*
===========================================================================
AAS_CountForcedClusterPortals
  Counts the number of areas forcibly marked as cluster portals, potentially
  for diagnostics or map analysis purposes.
  
  Parameters: None.
  
  Returns: Void. Outputs the count of forced cluster portals to the console.
  
  Changes Globals: None directly, but uses global aasworld data for counting.
===========================================================================
*/
void AAS_CountForcedClusterPortals(void) {
	int num, i;

	num = 0;
	for (i = 1; i < aasworld->numareas; i++) {
		if (aasworld->areasettings[i].contents & AREACONTENTS_CLUSTERPORTAL) {
			num++;
		}
	}
	botimport.Print(PRT_MESSAGE, "%6d forced portals\n", num);
}

/*
===========================================================================
AAS_CreateViewPortals
 Marks all areas designated as cluster portals also as view portals, likely
 for further processing or visibility calculations within the clustering process.

 Parameters: None.

 Returns: Void.

 Changes Globals:
 - Modifies the areasettings of aasworld to add AREACONTENTS_VIEWPORTAL flags
   where AREACONTENTS_CLUSTERPORTAL is set.
===========================================================================
*/
void AAS_CreateViewPortals(void) {
	int i;

	for (i = 1; i < aasworld->numareas; i++) {
		if (aasworld->areasettings[i].contents & AREACONTENTS_CLUSTERPORTAL) {
			aasworld->areasettings[i].contents |= AREACONTENTS_VIEWPORTAL;
		}
	}
}

/*
===========================================================================
AAS_SetViewPortalsAsClusterPortals
 Designates all view portals as cluster portals, likely for a different phase
 of map processing or optimization, ensuring navigational consistency.

 Parameters: None.

 Returns: Void.

 Changes Globals:
 - Updates the areasettings of aasworld, setting AREACONTENTS_CLUSTERPORTAL flags
   where AREACONTENTS_VIEWPORTAL is already set.
===========================================================================
*/
void AAS_SetViewPortalsAsClusterPortals(void) {
	int i;

	for (i = 1; i < aasworld->numareas; i++) {
		if (aasworld->areasettings[i].contents & AREACONTENTS_VIEWPORTAL) {
			aasworld->areasettings[i].contents |= AREACONTENTS_CLUSTERPORTAL;
		}
	}
}
/*
===========================================================================
AAS_InitClustering
 Initializes the clustering process for AI pathfinding by setting up portals
 and clusters based on the current game world state. This involves removing
 outdated cluster information, finding potential cluster portals, creating
 view portals, and evaluating the resulting clusters and portals for viability.

 Parameters: None.

 Returns: Void.

 Changes Globals:
 - Modifies global aasworld structures, including areas, portals, and clusters,
   to reflect the new clustering configuration.
===========================================================================
*/
void AAS_InitClustering(void) {
	int i, removedPortalAreas;
	int n, total, numreachabilityareas;

	// Exit if the world hasn't been loaded or if clustering isn't required
	if (!aasworld->loaded || (aasworld->numclusters >= 1 && !LibVarGetValue("forceclustering") && !LibVarGetValue("forcereachability"))) {
		return;
	}

	// Count and report on forced cluster portals
	AAS_CountForcedClusterPortals();

	// Prepare the game world for clustering
	AAS_RemoveClusterAreas(); // Removes all area cluster markings
	AAS_FindPossiblePortals(); // Identifies potential cluster portals
	AAS_CreateViewPortals(); // Creates view portals for bots

	// Initialize memory for portals and clusters
	if (aasworld->portals) {
		FreeMemory(aasworld->portals);
	}
	aasworld->portals = (aas_portal_t*)GetClearedMemory(AAS_MAX_PORTALS * sizeof(aas_portal_t));

	if (aasworld->portalindex) {
		FreeMemory(aasworld->portalindex);
	}
	aasworld->portalindex = (aas_portalindex_t*)GetClearedMemory(AAS_MAX_PORTALINDEXSIZE * sizeof(aas_portalindex_t));

	if (aasworld->clusters) {
		FreeMemory(aasworld->clusters);
	}
	aasworld->clusters = (aas_cluster_t*)GetClearedMemory(AAS_MAX_CLUSTERS * sizeof(aas_cluster_t));

	// Initialize clustering process
	removedPortalAreas = 0;
	do {
		// Reset portal and cluster counts
		aasworld->numportals = 1; // Portal 0 is a dummy
		aasworld->portalindexsize = 0;
		aasworld->numclusters = 1; // Cluster 0 is a dummy

		AAS_CreatePortals(); // Create the portals from identified areas

		// Attempt to find clusters with the current portal setup
		if (!AAS_FindClusters() || !AAS_TestPortals()) {
			removedPortalAreas++; // Adjust if clusters or portals are invalid and retry
			continue;
		}

		break; // Exit loop if successful
	} while (qtrue);

	// Mark AAS file for saving due to changes
	aasworld->savefile = qtrue;

	// Report on the clustering outcome
	botimport.Print(PRT_MESSAGE, "%6d portals created\n", aasworld->numportals);
	botimport.Print(PRT_MESSAGE, "%6d clusters created\n", aasworld->numclusters);
	for (i = 1; i < aasworld->numclusters; i++) {
		botimport.Print(PRT_MESSAGE, "cluster %d has %d reachability areas\n", i, aasworld->clusters[i].numreachabilityareas);
	}

	// Report on AAS file efficiency
	numreachabilityareas = 0;
	total = 0;
	for (i = 0; i < aasworld->numclusters; i++) {
		n = aasworld->clusters[i].numreachabilityareas;
		numreachabilityareas += n;
		total += n * n;
	}
	total += numreachabilityareas * aasworld->numportals;
	botimport.Print(PRT_MESSAGE, "%6i total reachability areas\n", numreachabilityareas);
	botimport.Print(PRT_MESSAGE, "%6i AAS memory/CPU usage (the lower the better)\n", total * 3);
}
