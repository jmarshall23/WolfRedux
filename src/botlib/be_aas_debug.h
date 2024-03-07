// be_aas_debug.h
//

/**
 * Clears the debug lines that have been drawn.
 */
void AAS_ClearShownDebugLines(void);

/**
 * Clears the shown polygons.
 */
void AAS_ClearShownPolygons(void);

/**
 * Shows a debug line from start to end with the specified color.
 *
 * @param start The start position of the line.
 * @param end The end position of the line.
 * @param color The color of the line.
 */
void AAS_DebugLine(vec3_t start, vec3_t end, int color);

/**
 * Shows a permanent line from start to end with the specified color.
 *
 * @param start The start position of the line.
 * @param end The end position of the line.
 * @param color The color of the line.
 */
void AAS_PermanentLine(vec3_t start, vec3_t end, int color);

/**
 * Shows a permanent cross at the given origin with the specified size and color.
 *
 * @param origin The origin of the cross.
 * @param size The size of the cross.
 * @param color The color of the cross.
 */
void AAS_DrawPermanentCross(vec3_t origin, float size, int color);

/**
 * Draws a cross in the plane at the given point with specified normal, distance, type, and color.
 *
 * @param point The point where the cross will be drawn.
 * @param normal The normal vector of the plane.
 * @param dist The distance from the point to the plane.
 * @param type The type of the cross to draw.
 * @param color The color of the cross.
 */
void AAS_DrawPlaneCross(vec3_t point, vec3_t normal, float dist, int type, int color);

/**
 * Shows a bounding box at the given origin with specified minimum and maximum bounds.
 *
 * @param origin The origin of the bounding box.
 * @param mins The minimum bounds of the bounding box.
 * @param maxs The maximum bounds of the bounding box.
 */
void AAS_ShowBoundingBox(vec3_t origin, vec3_t mins, vec3_t maxs);

/**
 * Shows the specified face.
 *
 * @param facenum The number of the face to show.
 */
void AAS_ShowFace(int facenum);

/**
 * Shows the specified area. If groundfacesonly is set, only ground faces are shown.
 *
 * @param areanum The number of the area to show.
 * @param groundfacesonly Specifies whether to show only ground faces.
 */
void AAS_ShowArea(int areanum, int groundfacesonly);

/**
 * Shows area polygons with the specified color. If groundfacesonly is set, only ground faces are shown.
 *
 * @param areanum The number of the area.
 * @param color The color of the polygons.
 * @param groundfacesonly Specifies whether to show only ground faces.
 */
void AAS_ShowAreaPolygons(int areanum, int color, int groundfacesonly);

/**
 * Draws a cross at the given origin with the specified size and color.
 *
 * @param origin The origin of the cross.
 * @param size The size of the cross.
 * @param color The color of the cross.
 */
void AAS_DrawCross(vec3_t origin, float size, int color);

/**
 * Prints the travel type.
 *
 * @param traveltype The travel type to print.
 */
void AAS_PrintTravelType(int traveltype);

/**
 * Draws an arrow from start to end with the specified line color and arrow color.
 *
 * @param start The start position of the arrow.
 * @param end The end position of the arrow.
 * @param linecolor The color of the line.
 * @param arrowcolor The color of the arrow.
 */
void AAS_DrawArrow(vec3_t start, vec3_t end, int linecolor, int arrowcolor);

/**
 * Visualizes the given reachability.
 *
 * @param reach A pointer to the reachability structure to visualize.
 */
void AAS_ShowReachability(struct aas_reachability_s* reach);

/**
 * Shows the reachable areas from the given area.
 *
 * @param areanum The number of the area from which to show reachable areas.
 */
void AAS_ShowReachableAreas(int areanum);

