#ifndef HELPERDEFINITIONS_H
#define HELPERDEFINITIONS_H

#include <raylib.h>

#define RENDER_BEGIN(bgColor) BeginDrawing();\
					 ClearBackground(bgColor);

#define RENDER_END	EndDrawing();

#define SCREEN_CENT (Vector2) {.x = GetScreenWidth() / 2, .y = GetScreenHeight() / 2}

#define PTR_FREE(ptr) free(ptr); ptr = NULL

#ifdef  _DEBUG
#define PTR_VALIDATE(ptr, doIfNull) \
	if(!ptr) \
		{ TraceLog(LOG_ERROR, "PTR %s was null in File: %s at line: %d", #ptr, __FILE__, __LINE__); doIfNull}
#else
#define PTR_VALIDATE(ptr, doIfNull)\
	if (!ptr) doIfNull; 
#endif
#endif