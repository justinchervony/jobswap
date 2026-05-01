#include "Jobswap.h"

__declspec(dllexport) IPlugin* __stdcall expCreatePlugin(const char* args)
{
    UNREFERENCED_PARAMETER(args);
    return (IPlugin*)(new Jobswap());
}

__declspec(dllexport) double __stdcall expGetInterfaceVersion(void)
{
    return ASHITA_INTERFACE_VERSION;
}

bool Jobswap::Initialize(IAshitaCore* core, ILogManager* logger, const uint32_t id)
{
    m_AshitaCore = core;
    m_LogManager = logger;
    m_PluginId   = id;
    pOutput      = new OutputHelpers(core, logger, GetName());
    pPacket      = new safePacketInjector(core->GetPacketManager());
    return true;
}

void Jobswap::Release(void)
{
    delete pPacket;
    delete pOutput;
}