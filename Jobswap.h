#ifndef __ASHITA_Jobswap_H_INCLUDED__
#define __ASHITA_Jobswap_H_INCLUDED__

#include "Ashita.h"
#include "Output.h"
#include "safePacketInjector.h"
#include "Utilities.h"
#include <map>
#include <string>

class Jobswap : IPlugin
{
private:
    IAshitaCore* m_AshitaCore;
    ILogManager* m_LogManager;
    uint32_t m_PluginId;
    OutputHelpers* pOutput;
    safePacketInjector* pPacket;

    struct cistringcmp
    {
        bool operator()(const string& lhs, const string& rhs) const
        {
            return (_stricmp(lhs.c_str(), rhs.c_str()) < 0);
        }
    };

    const std::map<std::string, uint8_t, cistringcmp> mJobMap = {
        {"WAR",1},{"MNK",2},{"WHM",3},{"BLM",4},{"RDM",5},
        {"THF",6},{"PLD",7},{"DRK",8},{"BST",9},{"BRD",10},
        {"RNG",11},{"SAM",12},{"NIN",13},{"DRG",14},{"SMN",15},
        {"BLU",16},{"COR",17},{"PUP",18},{"DNC",19},{"SCH",20}
    };

public:
    const char* GetName(void) const override { return "Jobswap"; }
    const char* GetAuthor(void) const override { return "Your Name"; }
    const char* GetDescription(void) const override { return "Change jobs via command."; }
    const char* GetLink(void) const override { return ""; }
    double GetVersion(void) const override { return 1.0f; }
    int32_t GetPriority(void) const override { return 0; }
    uint32_t GetFlags(void) const override { return (uint32_t)Ashita::PluginFlags::Legacy; }

    bool Initialize(IAshitaCore* core, ILogManager* logger, const uint32_t id) override;
    void Release(void) override;
    bool HandleCommand(int32_t mode, const char* command, bool injected) override;
    void ChangeJob(uint8_t mainJob, uint8_t subJob);
};
#endif