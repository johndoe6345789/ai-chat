/** @brief nextra-ai microservice entry point. */
#include "service-host/ServiceApp.h"

int main(int argc, char* argv[])
{
    const char* cfg = argc > 1
        ? argv[1] : "config/config.json";
    services::runService(cfg, "nextra-ai");
    return 0;
}
