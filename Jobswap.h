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
    uint32_t            m_PluginId;
    OutputHelpers* pOutput;
    safePacketInjector* pPacket;

    // -----------------------------------------------------------------------
    // Profile system
    // -----------------------------------------------------------------------
    struct JobProfile
    {
        uint8_t mainJob;
        uint8_t subJob;
    };

    std::map<std::string, JobProfile> mProfiles;

    void        LoadSettings(void);
    void        SaveSettings(void);
    std::string GetSettingsPath(void) const;
    bool        m_settingsLoaded = false;
    std::string m_characterName;

    // -----------------------------------------------------------------------
    // Rollback system
    // After a swap packet is sent, we count down ~90 frames (~1.5s at 60fps)
    // in Direct3DPresent then check if the server actually applied the change.
    // If not, we resend the original jobs to correct any gear-swap side effects
    // caused by LuAshitacast reacting to the outgoing packet before the server
    // rejects it.
    // -----------------------------------------------------------------------
    bool    m_pendingSwap = false;
    uint8_t m_pendingMain = 0;    // job we tried to swap TO
    uint8_t m_pendingSub = 0;
    uint8_t m_rollbackMain = 0;    // job we were on BEFORE the swap
    uint8_t m_rollbackSub = 0;
    int32_t m_swapTicksLeft = 0;    // countdown frames

    // Reverse lookup: job ID -> abbreviation string (for display)
    std::string JobIdToName(uint8_t id) const;

    // -----------------------------------------------------------------------
    // Case-insensitive string comparator (used by mJobMap)
    // -----------------------------------------------------------------------
    struct cistringcmp
    {
        bool operator()(const std::string& lhs, const std::string& rhs) const
        {
            return (_stricmp(lhs.c_str(), rhs.c_str()) < 0);
        }
    };

    const std::map<std::string, uint8_t, cistringcmp> mJobMap = {
        {"WAR", 1}, {"MNK", 2}, {"WHM", 3},  {"BLM", 4},  {"RDM", 5},
        {"THF", 6}, {"PLD", 7}, {"DRK", 8},  {"BST", 9},  {"BRD", 10},
        {"RNG",11}, {"SAM",12}, {"NIN",13},  {"DRG",14},  {"SMN",15},
        {"BLU",16}, {"COR",17}, {"PUP",18},  {"DNC",19},  {"SCH",20}
    };

public:
    const char* GetName(void)        const override { return "Jobswap"; }
    const char* GetAuthor(void)      const override { return "Ushikai"; }
    const char* GetDescription(void) const override { return "Change jobs via command with named profiles."; }
    const char* GetLink(void)        const override { return ""; }
    double      GetVersion(void)     const override { return 1.2f; }
    int32_t     GetPriority(void)    const override { return 0; }
    uint32_t    GetFlags(void)       const override { return (uint32_t)Ashita::PluginFlags::LegacyDirect3D; }

    bool Initialize(IAshitaCore* core, ILogManager* logger, const uint32_t id) override;
    void Release(void) override;
    bool HandleCommand(int32_t mode, const char* command, bool injected) override;
    void Direct3DPresent(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) override;
    bool Direct3DInitialize(IDirect3DDevice8* device) override;

    void ChangeJob(uint8_t mainJob, uint8_t subJob);
};

#endif