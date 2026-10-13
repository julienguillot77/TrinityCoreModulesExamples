#include "TrinityModule.h"
#include "../../include/GreetingService.h"
#include <cstdio>
#include <cstring>
#include <limits>

namespace
{
char prefix[128] = "Hello";
void ReadConfig(trinity_host const* host)
{
    auto const* api = TrinityHostApiV1(host);
    auto const* config = static_cast<trinity_config_api_v1 const*>(api->query.get_capability(const_cast<trinity_host*>(host), TRINITY_CAPABILITY_CONFIG_V1));
    if (config && config->size >= sizeof(*config) && config->abi_version == 1)
        config->get_string(const_cast<trinity_host*>(host), "Module.mod-greeting.Prefix", "Hello", prefix, sizeof(prefix));
}
int32_t Greet(void*, char const* subject, char* output, size_t capacity)
{
    if (!subject || (!output && capacity)) return -1;
    int required = std::snprintf(output, capacity, "%s, %s! mod-greeting is ready.", prefix, subject);
    return required < 0 || required == std::numeric_limits<int32_t>::max() ? -1 : required + 1;
}
void Startup(void*, trinity_host const* host)
{
    char message[512];
    Greet(nullptr, "world", message, sizeof(message));
    auto const* api = TrinityHostApiV1(host);
    api->log.write(const_cast<trinity_host*>(host), 1, message, std::strlen(message));
}
void Shutdown(void*, trinity_host const* host)
{
    auto const* api = TrinityHostApiV1(host);
    constexpr char message[] = "mod-greeting stopped.";
    api->log.write(const_cast<trinity_host*>(host), 1, message, sizeof(message) - 1);
}
int32_t Load(void*, trinity_host const* host)
{
    auto const* api = TrinityHostApiV1(host);
    if (!api || api->size < sizeof(*api) || api->abi_version != 1 || !api->query.register_service || !api->query.register_family || !api->query.get_capability) return -1;
    ReadConfig(host);
    static TrinityGreetingService service{sizeof(service), 1, nullptr, Greet};
    trinity_service_registration registration{sizeof(registration), TRINITY_EXAMPLE_GREETING_ID, "trinity.example.greeting.v1", &service};
    if (api->query.register_service(const_cast<trinity_host*>(host), &registration)) return -1;
    trinity_world_callbacks world{};
    world.size = sizeof(world);
    world.host = host;
    world.OnStartup = Startup;
    world.OnShutdown = Shutdown;
    return api->query.register_family(const_cast<trinity_host*>(host), TRINITY_FAMILY_WORLD, &world);
}
void Unload(void*, trinity_host const*) { std::strcpy(prefix, "Hello"); }
void Reload(void*, trinity_host const* host) { ReadConfig(host); }
}
extern "C" int32_t TrinityModuleGetApi(uint32_t version, trinity_module_api* output)
{
    if (version != TRINITY_ABI_V1 || !output) return -1;
    *output = {sizeof(*output), TRINITY_ABI_V1, nullptr, Load, Unload, Reload};
    return 0;
}
