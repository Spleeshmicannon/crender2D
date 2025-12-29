/* zlib license
 * Copyright (C) 2025-11-20 19:12:26 J. Benson
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution. 
*/

#ifndef VKH_MEMORY_H
#define VKH_MEMORY_H

#include <stddef.h>
#include <stdbool.h>
#include <cplat.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    void* arenaStart;
    void* head;
    size_t arenaSize;
}
CR_Arena;

CP_INLINE bool CR_createArena(CR_Arena*const arena, const size_t arenaSize)
{
    bool success = false;

    arena->arenaStart = CP_sysAllocate(arenaSize);
    arena->arenaSize = arenaSize;
    arena->head = arena->arenaStart;

    success = arena->arenaStart != NULL;
    
    return success;
}

CP_INLINE void CR_destoryArena(CR_Arena arena)
{
    CP_sysFree(arena.arenaStart);
}

CP_INLINE void* CR_arenaAllocate(CR_Arena* arena, size_t bytes)
{
    void* oldHead = arena->head; 

    if(((size_t)(oldHead - arena->arenaStart) + bytes) >= arena->arenaSize)
    {
        CP_log_error("Failed to allocate to arena");
        exit(-1);
        return NULL;
    }

    arena->head += bytes;

    CP_log_info("allocation space used: %d / %d", (size_t)(arena->head - arena->arenaStart), arena->arenaSize);

    return oldHead;
}

#ifdef __cplusplus
}
#endif

#endif // VKH_MEMORY_H

