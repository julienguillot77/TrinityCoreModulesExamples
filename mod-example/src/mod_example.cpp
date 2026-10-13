#include "TrinityModule.h"
#include "../../include/GreetingService.h"
#include <cstring>

namespace
{
trinity_host const* moduleHost = nullptr;
char subject[128] = "module SDK";
void ReadConfig(trinity_host const* host)
{
    auto const* api = TrinityHostApiV1(host);
    auto const* config = static_cast<trinity_config_api_v1 const*>(api->query.get_capability(const_cast<trinity_host*>(host), TRINITY_CAPABILITY_CONFIG_V1));
    if (config && config->size >= sizeof(*config) && config->abi_version == 1)
        config->get_string(const_cast<trinity_host*>(host), "Module.mod-example.Subject", "module SDK", subject, sizeof(subject));
}
void Startup(void*, trinity_host const* host)
{
    constexpr char message[] = "mod-example started; use 'module example hello'.";
    TrinityHostApiV1(host)->log.write(const_cast<trinity_host*>(host), 1, message, sizeof(message) - 1);
}
void Shutdown(void*, trinity_host const* host)
{
    constexpr char message[] = "mod-example stopped.";
    TrinityHostApiV1(host)->log.write(const_cast<trinity_host*>(host), 1, message, sizeof(message) - 1);
}
int32_t Hello(void*, trinity_command_context* command, char const*, size_t)
{
    auto const* api = TrinityHostApiV1(moduleHost);
    auto const* greeting = static_cast<TrinityGreetingService const*>(api->query.get_service(const_cast<trinity_host*>(moduleHost), TRINITY_EXAMPLE_GREETING_ID));
    auto const* commands = static_cast<trinity_command_api_v1 const*>(api->query.get_capability(const_cast<trinity_host*>(moduleHost), TRINITY_CAPABILITY_COMMAND_V1));
    if (!greeting || greeting->size < sizeof(*greeting) || greeting->version != 1 || !commands || commands->size < sizeof(*commands)) return -1;
    char message[512];
    int32_t required = greeting->greet(greeting->context, subject, message, sizeof(message));
    if (required < 1 || static_cast<size_t>(required) > sizeof(message)) return -1;
    commands->send_sys_message(command, message, static_cast<size_t>(required - 1));
    return 0;
}
int32_t Load(void*, trinity_host const* host)
{
    auto const* api = TrinityHostApiV1(host);
    if (!api || api->size < sizeof(*api) || api->abi_version != 1 || !api->query.get_service || !api->query.get_capability || !api->query.register_command || !api->query.register_family) return -1;
    if (!api->query.get_service(const_cast<trinity_host*>(host), TRINITY_EXAMPLE_GREETING_ID)) return -1;
    moduleHost = host;
    ReadConfig(host);
    trinity_world_callbacks world{};
    world.size = sizeof(world);
    world.host = host;
    world.OnStartup = Startup;
    world.OnShutdown = Shutdown;
    if (api->query.register_family(const_cast<trinity_host*>(host), TRINITY_FAMILY_WORLD, &world)) return -1;
    trinity_command_registration command{sizeof(command), "module example hello", "Print greeting service response.", Hello, nullptr, 371, 1};
    return api->query.register_command(const_cast<trinity_host*>(host), &command);
}
void Unload(void*, trinity_host const*) { moduleHost = nullptr; std::strcpy(subject, "module SDK"); }
void Reload(void*, trinity_host const* host) { ReadConfig(host); }
}
extern "C" int32_t TrinityModuleGetApi(uint32_t version, trinity_module_api* output)
{
    if (version != TRINITY_ABI_V1 || !output) return -1;
    *output = {sizeof(*output), TRINITY_ABI_V1, nullptr, Load, Unload, Reload};
    return 0;
}
