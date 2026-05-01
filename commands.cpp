#include "Jobswap.h"

bool Jobswap::HandleCommand(int32_t mode, const char* command, bool injected)
{
    UNREFERENCED_PARAMETER(mode);
    UNREFERENCED_PARAMETER(injected);

    std::vector<std::string> args;
    int argcount = Ashita::Commands::GetCommandArgs(command, &args);

    if (!(CheckArg(0, "/jobswap") || CheckArg(0, "/jswap")))
        return false;

    // /jobswap main <job>
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

    // /jobswap sub <job>
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

    // /jobswap <main> <sub>
    if (argcount >= 3)
    {
        auto mainIter = mJobMap.find(args[1]);
        auto subIter  = mJobMap.find(args[2]);
        if (mainIter == mJobMap.end() || subIter == mJobMap.end())
        {
            pOutput->error_f("Unknown job specified.");
            return true;
        }
        ChangeJob(mainIter->second, 0);
        ChangeJob(0, subIter->second);
        pOutput->message_f("Changing to $H%s$R/$H%s$R.", args[1].c_str(), args[2].c_str());
        return true;
    }

    pOutput->message("Usage: /jobswap main <job> | /jobswap sub <job> | /jobswap <main> <sub>");
    return true;
}

void Jobswap::ChangeJob(uint8_t mainJob, uint8_t subJob)
{
    uint8_t packet[8] = { 0 };
    packet[4] = mainJob;
    packet[5] = subJob;
    pPacket->addOutgoingPacket_s(0x100, 8, packet);
}