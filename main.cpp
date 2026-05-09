#include "Jobswap.h"
#include "rapidxml.hpp"
#include <fstream>
#include <sstream>

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
    m_PluginId = id;
    pOutput = new OutputHelpers(core, logger, GetName());
    pPacket = new safePacketInjector(core->GetPacketManager());

    //LoadSettings();
    //pOutput->message_f("Jobswap settings path: %s", GetSettingsPath().c_str());

    return true;
}

bool Jobswap::Direct3DInitialize(IDirect3DDevice8* device)
{
    UNREFERENCED_PARAMETER(device);
    return true;
}

void Jobswap::Release(void)
{
    SaveSettings();
    delete pPacket;
    delete pOutput;
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

std::string Jobswap::JobIdToName(uint8_t id) const
{
    for (const auto& pair : mJobMap)
    {
        if (pair.second == id)
            return pair.first;
    }
    return "???";
}

// Builds the per-character settings path, e.g.:
//   <AshitaRoot>/settings/jobswap/CharName.xml

std::string Jobswap::GetSettingsPath(void) const
{
    std::string path = m_AshitaCore->GetInstallPath();
    path += "config\\jobswap\\";
    ::CreateDirectoryA(path.c_str(), nullptr);
    path += m_characterName.empty() ? "default" : m_characterName;
    path += ".xml";
    return path;
}

// ---------------------------------------------------------------------------
// XML persistence
// ---------------------------------------------------------------------------

void Jobswap::LoadSettings(void)
{
    mProfiles.clear();

    std::string path = GetSettingsPath();

    std::ifstream file(path);
    if (!file.is_open())
        return; // No settings yet; first run is fine

    std::stringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();

    if (content.empty())
        return;

    rapidxml::xml_document<> doc;
    try
    {
        doc.parse<0>(&content[0]);
    }
    catch (...)
    {
        pOutput->error("Jobswap: Failed to parse settings XML.");
        return;
    }

    rapidxml::xml_node<>* root = doc.first_node("Jobswap");
    if (!root) return;

    rapidxml::xml_node<>* profiles = root->first_node("Profiles");
    if (!profiles) return;

    for (rapidxml::xml_node<>* node = profiles->first_node("Profile");
        node; node = node->next_sibling("Profile"))
    {
        rapidxml::xml_attribute<>* attrName = node->first_attribute("name");
        rapidxml::xml_attribute<>* attrMain = node->first_attribute("main");
        rapidxml::xml_attribute<>* attrSub = node->first_attribute("sub");

        if (!attrName || !attrMain || !attrSub)
            continue;

        std::string name = attrName->value();
        uint8_t mainJob = static_cast<uint8_t>(std::stoi(attrMain->value()));
        uint8_t subJob = static_cast<uint8_t>(std::stoi(attrSub->value()));

        mProfiles[name] = { mainJob, subJob };
    }

    pOutput->message_f("Jobswap: Loaded %d profile(s).", (int)mProfiles.size());
}

void Jobswap::SaveSettings(void)
{
    if (m_characterName.empty())
        return;

    std::string path = GetSettingsPath();

    std::ofstream file(path, std::ios::trunc);
    if (!file.is_open())
    {
        pOutput->error("Jobswap: Could not open settings file for writing.");
        return;
    }

    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<Jobswap>\n";
    file << "    <Profiles>\n";

    for (const auto& pair : mProfiles)
    {
        file << "        <Profile name=\"" << pair.first << "\""
            << " main=\"" << (int)pair.second.mainJob << "\""
            << " sub=\"" << (int)pair.second.subJob << "\" />\n";
    }

    file << "    </Profiles>\n";
    file << "</Jobswap>\n";
}