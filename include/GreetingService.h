#ifndef TRINITY_EXAMPLES_GREETING_SERVICE_H
#define TRINITY_EXAMPLES_GREETING_SERVICE_H
#include <stdint.h>
#include <stddef.h>
#define TRINITY_EXAMPLE_GREETING_ID UINT32_C(0x47524545)
/* Returns bytes required including terminator; negative on failure.
 * Output belongs to the caller. The service table lives until provider unload. */
typedef struct TrinityGreetingService
{
    uint32_t size;
    uint32_t version;
    void* context;
    int32_t (*greet)(void*, const char*, char*, size_t);
} TrinityGreetingService;
#endif
