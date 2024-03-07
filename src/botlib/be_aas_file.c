// be_aas_file.c
//

#include "../game/q_shared.h"
#include "../botutil/l_memory.h"
#include "../botutil/l_script.h"
#include "../botutil/l_precomp.h"
#include "../botutil/l_struct.h"
#include "../botutil/l_libvar.h"
#include "../botutil/l_utils.h"
#include "aasfile.h"
#include "../game/botlib.h"
#include "../game/be_aas.h"
#include "be_aas_funcs.h"
#include "be_interface.h"
#include "be_aas_def.h"

/*
==============================
  AAS_SwapAASData
	--Function to swap AAS data based on the system's endianness.
	--It ensures data integrity across different hardware architectures.
==============================
*/
void AAS_SwapAASData( void ) {
	int i, j;

	// If this OS doesn't need byte swapping, return
	if (LittleLong(1) == 1) {
		return;
	}

	// Bounding boxes
	for (i = 0; i < aasworld->numbboxes; i++) {
		aasworld->bboxes[i].presencetype = LittleLong(aasworld->bboxes[i].presencetype);
		aasworld->bboxes[i].flags = LittleLong(aasworld->bboxes[i].flags);
		for (j = 0; j < 3; j++) {
			aasworld->bboxes[i].mins[j] = LittleFloat(aasworld->bboxes[i].mins[j]);
			aasworld->bboxes[i].maxs[j] = LittleFloat(aasworld->bboxes[i].maxs[j]);
		}
	}

	// Vertexes
	for (i = 0; i < aasworld->numvertexes; i++) {
		for (j = 0; j < 3; j++)
			aasworld->vertexes[i][j] = LittleFloat(aasworld->vertexes[i][j]);
	}

	// Planes
	for (i = 0; i < aasworld->numplanes; i++) {
		for (j = 0; j < 3; j++)
			aasworld->planes[i].normal[j] = LittleFloat(aasworld->planes[i].normal[j]);
		aasworld->planes[i].dist = LittleFloat(aasworld->planes[i].dist);
		aasworld->planes[i].type = LittleLong(aasworld->planes[i].type);
	}

	// Edges
	for (i = 0; i < aasworld->numedges; i++) {
		aasworld->edges[i].v[0] = LittleLong(aasworld->edges[i].v[0]);
		aasworld->edges[i].v[1] = LittleLong(aasworld->edges[i].v[1]);
	}

	// Edge index
	for (i = 0; i < aasworld->edgeindexsize; i++) {
		aasworld->edgeindex[i] = LittleLong(aasworld->edgeindex[i]);
	}

	// Faces
	for (i = 0; i < aasworld->numfaces; i++) {
		aasworld->faces[i].planenum = LittleLong(aasworld->faces[i].planenum);
		aasworld->faces[i].faceflags = LittleLong(aasworld->faces[i].faceflags);
		aasworld->faces[i].numedges = LittleLong(aasworld->faces[i].numedges);
		aasworld->faces[i].firstedge = LittleLong(aasworld->faces[i].firstedge);
		aasworld->faces[i].frontarea = LittleLong(aasworld->faces[i].frontarea);
		aasworld->faces[i].backarea = LittleLong(aasworld->faces[i].backarea);
	}

	// Face index
	for (i = 0; i < aasworld->faceindexsize; i++) {
		aasworld->faceindex[i] = LittleLong(aasworld->faceindex[i]);
	}

	// Convex areas
	for (i = 0; i < aasworld->numareas; i++) {
		aasworld->areas[i].areanum = LittleLong(aasworld->areas[i].areanum);
		aasworld->areas[i].numfaces = LittleLong(aasworld->areas[i].numfaces);
		aasworld->areas[i].firstface = LittleLong(aasworld->areas[i].firstface);
		for (j = 0; j < 3; j++) {
			aasworld->areas[i].mins[j] = LittleFloat(aasworld->areas[i].mins[j]);
			aasworld->areas[i].maxs[j] = LittleFloat(aasworld->areas[i].maxs[j]);
			aasworld->areas[i].center[j] = LittleFloat(aasworld->areas[i].center[j]);
		}
	}

	// Area settings
	for (i = 0; i < aasworld->numareasettings; i++) {
		aasworld->areasettings[i].contents = LittleLong(aasworld->areasettings[i].contents);
		aasworld->areasettings[i].areaflags = LittleLong(aasworld->areasettings[i].areaflags);
		aasworld->areasettings[i].presencetype = LittleLong(aasworld->areasettings[i].presencetype);
		aasworld->areasettings[i].cluster = LittleLong(aasworld->areasettings[i].cluster);
		aasworld->areasettings[i].clusterareanum = LittleLong(aasworld->areasettings[i].clusterareanum);
		aasworld->areasettings[i].numreachableareas = LittleLong(aasworld->areasettings[i].numreachableareas);
		aasworld->areasettings[i].firstreachablearea = LittleLong(aasworld->areasettings[i].firstreachablearea);
		aasworld->areasettings[i].groundsteepness = LittleLong(aasworld->areasettings[i].groundsteepness);
	}

	// Area reachability
	for (i = 0; i < aasworld->reachabilitysize; i++) {
		aasworld->reachability[i].areanum = LittleLong(aasworld->reachability[i].areanum);
		aasworld->reachability[i].facenum = LittleLong(aasworld->reachability[i].facenum);
		aasworld->reachability[i].edgenum = LittleLong(aasworld->reachability[i].edgenum);
		for (j = 0; j < 3; j++) {
			aasworld->reachability[i].start[j] = LittleFloat(aasworld->reachability[i].start[j]);
			aasworld->reachability[i].end[j] = LittleFloat(aasworld->reachability[i].end[j]);
		}
		aasworld->reachability[i].traveltype = LittleLong(aasworld->reachability[i].traveltype);
		aasworld->reachability[i].traveltime = LittleShort(aasworld->reachability[i].traveltime);
	}

	// Nodes
	for (i = 0; i < aasworld->numnodes; i++) {
		aasworld->nodes[i].planenum = LittleLong(aasworld->nodes[i].planenum);
		aasworld->nodes[i].children[0] = LittleLong(aasworld->nodes[i].children[0]);
		aasworld->nodes[i].children[1] = LittleLong(aasworld->nodes[i].children[1]);
	}

	// Cluster portals
	for (i = 0; i < aasworld->numportals; i++) {
		aasworld->portals[i].areanum = LittleLong(aasworld->portals[i].areanum);
		aasworld->portals[i].frontcluster = LittleLong(aasworld->portals[i].frontcluster);
		aasworld->portals[i].backcluster = LittleLong(aasworld->portals[i].backcluster);
		aasworld->portals[i].clusterareanum[0] = LittleLong(aasworld->portals[i].clusterareanum[0]);
		aasworld->portals[i].clusterareanum[1] = LittleLong(aasworld->portals[i].clusterareanum[1]);
	}

	// Cluster portal index
	for (i = 0; i < aasworld->portalindexsize; i++) {
		aasworld->portalindex[i] = LittleLong(aasworld->portalindex[i]);
	}

	// Cluster
	for (i = 0; i < aasworld->numclusters; i++) {
		aasworld->clusters[i].numareas = LittleLong(aasworld->clusters[i].numareas);
		aasworld->clusters[i].numreachabilityareas = LittleLong(aasworld->clusters[i].numreachabilityareas);
		aasworld->clusters[i].numportals = LittleLong(aasworld->clusters[i].numportals);
		aasworld->clusters[i].firstportal = LittleLong(aasworld->clusters[i].firstportal);
	}
} 

/*
==========================
	AAS_DumpAASData
		--Function to release all loaded AAS data from memory, effectively "dumping" the current AAS state.
		--This is useful for cleanup or before loading a new AAS file.
==========================
*/
void AAS_DumpAASData( void ) {
	// Free memory for various AAS data structures
	aasworld->numbboxes = 0;
	if (aasworld->bboxes) {
		FreeMemory(aasworld->bboxes);
	}
	aasworld->bboxes = NULL;

	aasworld->numvertexes = 0;
	if (aasworld->vertexes) {
		FreeMemory(aasworld->vertexes);
	}
	aasworld->vertexes = NULL;

	aasworld->numplanes = 0;
	if (aasworld->planes) {
		FreeMemory(aasworld->planes);
	}
	aasworld->planes = NULL;

	aasworld->numedges = 0;
	if (aasworld->edges) {
		FreeMemory(aasworld->edges);
	}
	aasworld->edges = NULL;

	aasworld->edgeindexsize = 0;
	if (aasworld->edgeindex) {
		FreeMemory(aasworld->edgeindex);
	}
	aasworld->edgeindex = NULL;

	aasworld->numfaces = 0;
	if (aasworld->faces) {
		FreeMemory(aasworld->faces);
	}
	aasworld->faces = NULL;

	aasworld->faceindexsize = 0;
	if (aasworld->faceindex) {
		FreeMemory(aasworld->faceindex);
	}
	aasworld->faceindex = NULL;

	aasworld->numareas = 0;
	if (aasworld->areas) {
		FreeMemory(aasworld->areas);
	}
	aasworld->areas = NULL;

	aasworld->numareasettings = 0;
	if (aasworld->areasettings) {
		FreeMemory(aasworld->areasettings);
	}
	aasworld->areasettings = NULL;

	aasworld->reachabilitysize = 0;
	if (aasworld->reachability) {
		FreeMemory(aasworld->reachability);
	}
	aasworld->reachability = NULL;

	aasworld->numnodes = 0;
	if (aasworld->nodes) {
		FreeMemory(aasworld->nodes);
	}
	aasworld->nodes = NULL;

	aasworld->numportals = 0;
	if (aasworld->portals) {
		FreeMemory(aasworld->portals);
	}
	aasworld->portals = NULL;

	aasworld->numportals = 0;
	if (aasworld->portalindex) {
		FreeMemory(aasworld->portalindex);
	}
	aasworld->portalindex = NULL;

	aasworld->portalindexsize = 0;
	if (aasworld->clusters) {
		FreeMemory(aasworld->clusters);
	}
	aasworld->clusters = NULL;

	aasworld->numclusters = 0;

	// Reset flags
	aasworld->loaded = qfalse;
	aasworld->initialized = qfalse;
	aasworld->savefile = qfalse;
} 


/*
============================
	AAS_LoadAASLump
		-Loads a specific section (lump) of an AAS file, such as vertices or edges.
		-A lump represents a chunk of data within the AAS file structure.
		-Parameters:
		-- fp: The file handle for the AAS file.
		-- offset: The offset in the file where the lump starts.
		-- length: The length of the lump in bytes.
		-- lastoffset: The last read offset in the file, used to optimize sequential reads.
		-Returns a pointer to the loaded data if successful, or NULL if there was no data to load or an error occurred.
============================
*/
char* AAS_LoadAASLump(fileHandle_t fp, int offset, int length, int* lastoffset) {
	// If there's no data to read (length is 0), return NULL immediately.
	if (!length) {
		return NULL;
	}

	// Check if the requested read offset is different from the last read offset.
	// If it is, seek to the new offset, issuing a warning about non-sequential reads.
	if (offset != *lastoffset) {
		botimport.Print(PRT_WARNING, "AAS file not sequentially read\n");
		if (botimport.FS_Seek(fp, offset, FS_SEEK_SET)) {
			// If seeking to the new offset fails, log an error, dump the currently loaded AAS data, close the file, and return NULL.
			AAS_Error("Can't seek to AAS lump\n");
			AAS_DumpAASData();
			botimport.FS_FCloseFile(fp);
			return NULL;
		}
	}

	// Allocate memory for the lump data, adding one extra byte for a null terminator (useful if data is text).
	char* buf = (char*)GetClearedHunkMemory(length + 1);

	// Read the lump data from the file into the allocated buffer.
	if (length) {
		botimport.FS_Read(buf, length, fp);
		// Update the last offset based on the amount of data read.
		*lastoffset += length;
	}

	// Return the buffer containing the loaded lump data.
	return buf;
}

/*
==============================
	AAS_DData
		-Decodes (or decrypts) AAS data in-place.
		-This function applies a simple XOR-based obfuscation algorithm to the data,
		-making it less readable outside of its intended context. It's commonly used to
		-prevent easy tampering or reading of sensitive information stored in memory.
		-Parameters:
		-- data: Pointer to the data to be decoded.
		-- size: The size of the data in bytes.
==============================
*/
void AAS_DData(unsigned char* data, int size) {
	// Iterate over each byte of the data.
	for (int i = 0; i < size; i++) {
		// Decode the current byte by XORing it with a value derived from its index.
		// The use of 119 as a multiplier in the XOR operation is arbitrary and acts as a simple key.
		// Changing the multiplier (key) would change the encoded data, providing a simple form of obfuscation.
		data[i] ^= (unsigned char)i * 119;
	}
}

/*
===============================
	AAS_LoadAASFile
		-Attempts to load an AAS (Area Awareness System) file specified by the filename.
		-AAS files are used for navigation data in game AI.
		-Returns an error code on failure or BLERR_NOERROR on success.
===============================
*/
int AAS_LoadAASFile( char *filename ) {
	fileHandle_t fp; // File pointer
    aas_header_t header; // AAS file header
    int offset, length, lastoffset;

    // Log attempt to load the file
    botimport.Print(PRT_MESSAGE, "Trying to load %s\n", filename);

    // Clear current AAS data before loading new data
    AAS_DumpAASData();

    // Open the AAS file
    botimport.FS_FOpenFile(filename, &fp, FS_READ);
    if (!fp) {
        AAS_Error("Can't open %s\n", filename);
        return BLERR_CANNOTOPENAASFILE;
    }

    // Read the AAS file header
    botimport.FS_Read(&header, sizeof(aas_header_t), fp);
    lastoffset = sizeof(aas_header_t);

    // Verify file identification and version
    header.ident = LittleLong(header.ident);
    if (header.ident != AASID) {
        AAS_Error("%s is not an AAS file\n", filename);
        botimport.FS_FCloseFile(fp);
        return BLERR_WRONGAASFILEID;
    }

    header.version = LittleLong(header.version);
    if (header.version != AASVERSION) {
        AAS_Error("AAS file %s is version %i, not %i\n", filename, header.version, AASVERSION);
        botimport.FS_FCloseFile(fp);
        return BLERR_WRONGAASFILEVERSION;
    }

    // Optional: Decode data if necessary
    if (header.version == AASVERSION) {
        AAS_DData((unsigned char *)&header + 8, sizeof(aas_header_t) - 8);
    }

    // Check if the AAS file matches the current map's checksum
    (*aasworld).bspchecksum = atoi(LibVarGetString("sv_mapChecksum"));
    if (LittleLong(header.bspchecksum) != (*aasworld).bspchecksum) {
        AAS_Error("AAS file %s is out of date\n", filename);
        botimport.FS_FCloseFile(fp);
        return BLERR_WRONGAASFILEVERSION;
    }

	// Load all lumps of data from the file
	// Each lump loading follows a similar pattern:
	// 1. Get lump offset and length
	// 2. Load lump data
	// 3. Update lump count
	// Repeat for each data lump (bounding boxes, vertices, planes, etc.)
	// Detailed comments for one lump as example, others follow similarly
	// Bounding boxes
	offset = LittleLong(header.lumps[AASLUMP_BBOXES].fileofs);
	length = LittleLong(header.lumps[AASLUMP_BBOXES].filelen);
	aasworld->bboxes = (aas_bbox_t*)AAS_LoadAASLump(fp, offset, length, &lastoffset);
	aasworld->numbboxes = length / sizeof(aas_bbox_t);
	if (aasworld->numbboxes && !aasworld->bboxes) {
		return BLERR_CANNOTREADAASLUMP;
	}

	// Vertexes
	offset = LittleLong(header.lumps[AASLUMP_VERTEXES].fileofs);
	length = LittleLong(header.lumps[AASLUMP_VERTEXES].filelen);
	aasworld->vertexes = (aas_vertex_t*)AAS_LoadAASLump(fp, offset, length, &lastoffset);
	aasworld->numvertexes = length / sizeof(aas_vertex_t);
	if (aasworld->numvertexes && !aasworld->vertexes) {
		return BLERR_CANNOTREADAASLUMP;
	}

	// Planes
	offset = LittleLong(header.lumps[AASLUMP_PLANES].fileofs);
	length = LittleLong(header.lumps[AASLUMP_PLANES].filelen);
	aasworld->planes = (aas_plane_t*)AAS_LoadAASLump(fp, offset, length, &lastoffset);
	aasworld->numplanes = length / sizeof(aas_plane_t);
	if (aasworld->numplanes && !aasworld->planes) {
		return BLERR_CANNOTREADAASLUMP;
	}

	// Edges
	offset = LittleLong(header.lumps[AASLUMP_EDGES].fileofs);
	length = LittleLong(header.lumps[AASLUMP_EDGES].filelen);
	aasworld->edges = (aas_edge_t*)AAS_LoadAASLump(fp, offset, length, &lastoffset);
	aasworld->numedges = length / sizeof(aas_edge_t);
	if (aasworld->numedges && !aasworld->edges) {
		return BLERR_CANNOTREADAASLUMP;
	}

	// Edge index
	offset = LittleLong(header.lumps[AASLUMP_EDGEINDEX].fileofs);
	length = LittleLong(header.lumps[AASLUMP_EDGEINDEX].filelen);
	aasworld->edgeindex = (aas_edgeindex_t*)AAS_LoadAASLump(fp, offset, length, &lastoffset);
	aasworld->edgeindexsize = length / sizeof(aas_edgeindex_t);
	if (aasworld->edgeindexsize && !aasworld->edgeindex) {
		return BLERR_CANNOTREADAASLUMP;
	}

	// Faces
	offset = LittleLong(header.lumps[AASLUMP_FACES].fileofs);
	length = LittleLong(header.lumps[AASLUMP_FACES].filelen);
	aasworld->faces = (aas_face_t*)AAS_LoadAASLump(fp, offset, length, &lastoffset);
	aasworld->numfaces = length / sizeof(aas_face_t);
	if (aasworld->numfaces && !aasworld->faces) {
		return BLERR_CANNOTREADAASLUMP;
	}

	// Face index
	offset = LittleLong(header.lumps[AASLUMP_FACEINDEX].fileofs);
	length = LittleLong(header.lumps[AASLUMP_FACEINDEX].filelen);
	aasworld->faceindex = (aas_faceindex_t*)AAS_LoadAASLump(fp, offset, length, &lastoffset);
	aasworld->faceindexsize = length / sizeof(int);
	if (aasworld->faceindexsize && !aasworld->faceindex) {
		return BLERR_CANNOTREADAASLUMP;
	}

	// Convex areas
	offset = LittleLong(header.lumps[AASLUMP_AREAS].fileofs);
	length = LittleLong(header.lumps[AASLUMP_AREAS].filelen);
	aasworld->areas = (aas_area_t*)AAS_LoadAASLump(fp, offset, length, &lastoffset);
	aasworld->numareas = length / sizeof(aas_area_t);
	if (aasworld->numareas && !aasworld->areas) {
		return BLERR_CANNOTREADAASLUMP;
	}

	// Area settings
	offset = LittleLong(header.lumps[AASLUMP_AREASETTINGS].fileofs);
	length = LittleLong(header.lumps[AASLUMP_AREASETTINGS].filelen);
	aasworld->areasettings = (aas_areasettings_t*)AAS_LoadAASLump(fp, offset, length, &lastoffset);
	aasworld->numareasettings = length / sizeof(aas_areasettings_t);
	if (aasworld->numareasettings && !aasworld->areasettings) {
		return BLERR_CANNOTREADAASLUMP;
	}

	// Reachability list
	offset = LittleLong(header.lumps[AASLUMP_REACHABILITY].fileofs);
	length = LittleLong(header.lumps[AASLUMP_REACHABILITY].filelen);
	aasworld->reachability = (aas_reachability_t*)AAS_LoadAASLump(fp, offset, length, &lastoffset);
	aasworld->reachabilitysize = length / sizeof(aas_reachability_t);
	if (aasworld->reachabilitysize && !aasworld->reachability) {
		return BLERR_CANNOTREADAASLUMP;
	}

	// Nodes
	offset = LittleLong(header.lumps[AASLUMP_NODES].fileofs);
	length = LittleLong(header.lumps[AASLUMP_NODES].filelen);
	aasworld->nodes = (aas_node_t*)AAS_LoadAASLump(fp, offset, length, &lastoffset);
	aasworld->numnodes = length / sizeof(aas_node_t);
	if (aasworld->numnodes && !aasworld->nodes) {
		return BLERR_CANNOTREADAASLUMP;
	}

	// Cluster portals
	offset = LittleLong(header.lumps[AASLUMP_PORTALS].fileofs);
	length = LittleLong(header.lumps[AASLUMP_PORTALS].filelen);
	aasworld->portals = (aas_portal_t*)AAS_LoadAASLump(fp, offset, length, &lastoffset);
	aasworld->numportals = length / sizeof(aas_portal_t);
	if (aasworld->numportals && !aasworld->portals) {
		return BLERR_CANNOTREADAASLUMP;
	}

	// Cluster portal index
	offset = LittleLong(header.lumps[AASLUMP_PORTALINDEX].fileofs);
	length = LittleLong(header.lumps[AASLUMP_PORTALINDEX].filelen);
	aasworld->portalindex = (aas_portalindex_t*)AAS_LoadAASLump(fp, offset, length, &lastoffset);
	aasworld->portalindexsize = length / sizeof(aas_portalindex_t);
	if (aasworld->portalindexsize && !aasworld->portalindex) {
		return BLERR_CANNOTREADAASLUMP;
	}

	// Clusters
	offset = LittleLong(header.lumps[AASLUMP_CLUSTERS].fileofs);
	length = LittleLong(header.lumps[AASLUMP_CLUSTERS].filelen);
	aasworld->clusters = (aas_cluster_t*)AAS_LoadAASLump(fp, offset, length, &lastoffset);
	aasworld->numclusters = length / sizeof(aas_cluster_t);
	if (aasworld->numclusters && !aasworld->clusters) {
		return BLERR_CANNOTREADAASLUMP;
	}

	// After loading all lumps, swap data if necessary for endianness
	AAS_SwapAASData();

	// Mark AAS data as loaded
	(*aasworld).loaded = qtrue;

	// Close the file
	botimport.FS_FCloseFile(fp);

	// Debugging information, if enabled
#ifdef AASFILEDEBUG
	AAS_FileInfo();
#endif

	return BLERR_NOERROR; // Successful load
}
