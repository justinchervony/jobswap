#include "Jobswap.h"

bool Jobswap::HandleCommand(int32_t mode, const char* command, bool injected)
{
    UNREFERENCED_PARAMETER(mode);
    UNREFERENCED_PARAMETER(injected);

    std::vector<std::string> args;
    int argcount = Ashita::Commands::GetCommandArgs(command, &args);

    if (!m_settingsLoaded)
    {
        const char* charName = m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberName(0);
        m_characterName = (charName && charName[0] != '\0') ? charName : "default";
        LoadSettings();
        m_settingsLoaded = true;
    }

    if (!(CheckArg(0, "/jobswap") || CheckArg(0, "/js")))
        return false;

    // -----------------------------------------------------------------------
    // /js save <profilename>
    // Snapshots the character's current main/sub jobs under a named profile.
    // Overwrites silently if it already exists.
    // -----------------------------------------------------------------------
    if (CheckArg(1, "save"))
    {
        if (argcount < 3)
        {
            pOutput->message("Usage: /js save <profilename>");
            return true;
        }

        const std::string profileName = args[2];

        auto* player = m_AshitaCore->GetMemoryManager()->GetPlayer();
        uint8_t curMain = static_cast<uint8_t>(player->GetMainJob());
        uint8_t curSub = static_cast<uint8_t>(player->GetSubJob());

        if (curMain == 0)
        {
            pOutput->error("Jobswap: Could not read current job. Are you logged in?");
            return true;
        }

        mProfiles[profileName] = { curMain, curSub };
        SaveSettings();

        pOutput->message_f("Profile '$H%s$R' saved: $H%s$R/$H%s$R.",
            profileName.c_str(),
            JobIdToName(curMain).c_str(),
            JobIdToName(curSub).c_str());

        return true;
    }

    // -----------------------------------------------------------------------
    // /js load <profilename>
    // Applies a named profile. Also reachable via /js <profilename> shorthand.
    // -----------------------------------------------------------------------
    if (CheckArg(1, "load"))
    {
        if (argcount < 3)
        {
            pOutput->message("Usage: /js load <profilename>");
            return true;
        }

        const std::string profileName = args[2];
        auto iter = mProfiles.find(profileName);
        if (iter == mProfiles.end())
        {
            pOutput->error_f("No profile named '$H%s$R'. Use /js profiles to list saved profiles.", profileName.c_str());
            return true;
        }

        const JobProfile& p = iter->second;
        ChangeJob(p.mainJob, p.subJob);
        pOutput->message_f("Loading profile '$H%s$R': $H%s$R/$H%s$R.",
            profileName.c_str(),
            JobIdToName(p.mainJob).c_str(),
            JobIdToName(p.subJob).c_str());

        return true;
    }

    // -----------------------------------------------------------------------
    // /js remove <profilename>
    // Deletes a named profile and saves immediately.
    // -----------------------------------------------------------------------
    if (CheckArg(1, "remove"))
    {
        if (argcount < 3)
        {
            pOutput->message("Usage: /js remove <profilename>");
            return true;
        }

        const std::string profileName = args[2];
        auto iter = mProfiles.find(profileName);
        if (iter == mProfiles.end())
        {
            pOutput->error_f("No profile named '$H%s$R' to remove.", profileName.c_str());
            return true;
        }

        mProfiles.erase(iter);
        SaveSettings();
        pOutput->message_f("Profile '$H%s$R' removed.", profileName.c_str());
        return true;
    }

    // -----------------------------------------------------------------------
    // /js profiles
    // Lists all saved profiles.
    // -----------------------------------------------------------------------
    if (CheckArg(1, "profiles"))
    {
        if (mProfiles.empty())
        {
            pOutput->message("No profiles saved. Use /js save <profilename> to create one.");
            return true;
        }

        pOutput->message("Saved profiles:");
        for (const auto& pair : mProfiles)
        {
            pOutput->message_f("  $H%s$R: %s/%s",
                pair.first.c_str(),
                JobIdToName(pair.second.mainJob).c_str(),
                JobIdToName(pair.second.subJob).c_str());
        }
        return true;
    }

    // -----------------------------------------------------------------------
    // /js main <job>
    // -----------------------------------------------------------------------
    if (CheckArg(1, "main") && argcount >= 3)
    {
        auto iter = mJobMap.find(args[2]);
        if (iter == mJobMap.end())
        {
            pOutput->error_f("Unknown job: [$H%s$R]", args[2].c_str());
            return true;
        }
        ChangeJob(iter->second, 0);
        pOutput->message_f("Changing main job to $H%s$R.", args[2].c_str());
        return true;
    }

    // -----------------------------------------------------------------------
    // /js sub <job>
    // -----------------------------------------------------------------------
    if (CheckArg(1, "sub") && argcount >= 3)
    {
        auto iter = mJobMap.find(args[2]);
        if (iter == mJobMap.end())
        {
            pOutput->error_f("Unknown job: [$H%s$R]", args[2].c_str());
            return true;
        }
        ChangeJob(0, iter->second);
        pOutput->message_f("Changing sub job to $H%s$R.", args[2].c_str());
        return true;
    }

    // -----------------------------------------------------------------------
    // /js <profilename>  (shorthand for /js load <profilename>)
    // Checked BEFORE the <main> <sub> path so single-word args hit profiles
    // first. If no profile matches it falls through to the usage message.
    // -----------------------------------------------------------------------
    if (argcount == 2)
    {
        const std::string profileName = args[1];
        auto iter = mProfiles.find(profileName);
        if (iter != mProfiles.end())
        {
            const JobProfile& p = iter->second;
            ChangeJob(p.mainJob, p.subJob);
            pOutput->message_f("Loading profile '$H%s$R': $H%s$R/$H%s$R.",
                profileName.c_str(),
                JobIdToName(p.mainJob).c_str(),
                JobIdToName(p.subJob).c_str());
            return true;
        }
        // Fall through to usage if no profile matched
    }

    // -----------------------------------------------------------------------
    // /js <main> <sub>
    // -----------------------------------------------------------------------
    if (argcount >= 3)
    {
        auto mainIter = mJobMap.find(args[1]);
        auto subIter = mJobMap.find(args[2]);
        if (mainIter == mJobMap.end() || subIter == mJobMap.end())
        {
            pOutput->error("Unknown job specified.");
            return true;
        }
        ChangeJob(mainIter->second, subIter->second);
        pOutput->message_f("Changing to $H%s$R/$H%s$R.", args[1].c_str(), args[2].c_str());
        return true;
    }

    // -----------------------------------------------------------------------
    // Usage
    // -----------------------------------------------------------------------
    pOutput->message("Jobswap commands:");
    pOutput->message("  /js <main> <sub>            - Swap to job combo");
    pOutput->message("  /js main <job>              - Swap main job only");
    pOutput->message("  /js sub <job>               - Swap sub job only");
    pOutput->message("  /js save <name>             - Save current jobs as a named profile");
    pOutput->message("  /js load <name>             - Load a named profile");
    pOutput->message("  /js <name>                  - Shorthand for load");
    pOutput->message("  /js profiles                - List all saved profiles");
    pOutput->message("  /js remove <name>           - Remove a saved profile");
    return true;
}

void Jobswap::ChangeJob(uint8_t mainJob, uint8_t subJob)
{
    auto* player = m_AshitaCore->GetMemoryManager()->GetPlayer();
    uint8_t resolvedMain = (mainJob != 0) ? mainJob : static_cast<uint8_t>(player->GetMainJob());
    uint8_t resolvedSub = (subJob != 0) ? subJob : static_cast<uint8_t>(player->GetSubJob());

    // Guard against main == sub, correcting sub to WAR (or MNK if main is WAR)
    if (resolvedMain != 0 && resolvedMain == resolvedSub)
    {
        resolvedSub = (resolvedMain == 1) ? 2 : 1;
        pOutput->message_f("Warning: main and sub cannot match. Sub defaulted to $H%s$R.",
            JobIdToName(resolvedSub).c_str());
    }

    // Store rollback state — what we're on now vs what we want
    m_rollbackMain = static_cast<uint8_t>(player->GetMainJob());
    m_rollbackSub = static_cast<uint8_t>(player->GetSubJob());
    m_pendingMain = resolvedMain;
    m_pendingSub = resolvedSub;
    m_swapTicksLeft = 90; // ~1.5s at 60fps
    m_pendingSwap = true;

    uint8_t packet[8] = { 0 };
    packet[4] = resolvedMain;
    packet[5] = resolvedSub;
    pPacket->addOutgoingPacket_s(0x100, 8, packet);
}

void Jobswap::Direct3DPresent(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
    UNREFERENCED_PARAMETER(pSourceRect);
    UNREFERENCED_PARAMETER(pDestRect);
    UNREFERENCED_PARAMETER(hDestWindowOverride);
    UNREFERENCED_PARAMETER(pDirtyRegion);

    if (!m_pendingSwap)
        return;

    if (--m_swapTicksLeft > 0)
        return;

    // Countdown expired — check if the server applied the swap
    auto* player = m_AshitaCore->GetMemoryManager()->GetPlayer();
    uint8_t curMain = static_cast<uint8_t>(player->GetMainJob());
    uint8_t curSub = static_cast<uint8_t>(player->GetSubJob());

    m_pendingSwap = false;

    if (curMain == m_pendingMain && curSub == m_pendingSub)
        return; // Server accepted it, nothing to do

    // Server rejected — resend original jobs to correct gear-swap side effects
    pOutput->message("Job swap was not applied. Location may not allow it. Restoring original jobs.");

    uint8_t packet[8] = { 0 };
    packet[4] = m_rollbackMain;
    packet[5] = m_rollbackSub;
    pPacket->addOutgoingPacket_s(0x100, 8, packet);
}