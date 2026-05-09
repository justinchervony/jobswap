# Jobswap

An Ashita v4 plugin for Final Fantasy XI that allows job swapping via chat commands, with support for named profiles per character.

## Requirements

- Ashita v4
- Must be used in a zone that permits job changes (Mog House, Nomad Moogle, etc.)

## Installation

Copy the compiled `Jobswap.dll` into your Ashita `plugins` folder and load it with:

```
/load jobswap
```

## Commands

All commands support both `/jobswap` and `/js`.

| Command | Description |
|---|---|
| `/js <main> <sub>` | Swap to a job combo directly |
| `/js main <job>` | Swap main job only |
| `/js sub <job>` | Swap sub job only |
| `/js save <name>` | Save current jobs as a named profile |
| `/js load <name>` | Load a named profile |
| `/js <name>` | Shorthand for load |
| `/js profiles` | List all saved profiles |
| `/js remove <name>` | Remove a saved profile |

## Examples

```
/js smn whm                 → Swap to SMN/WHM
/js main blm                → Swap main to BLM, keep current sub
/js save default            → Save current jobs as "default"
/js save leveling           → Save current jobs as "leveling"
/js default                 → Load the "default" profile
/js leveling                → Load the "leveling" profile
/js remove leveling         → Remove the "leveling" profile
/js profiles                → List all saved profiles
```

## Supported Jobs

WAR, MNK, WHM, BLM, RDM, THF, PLD, DRK, BST, BRD, RNG, SAM, NIN, DRG, SMN, BLU, COR, PUP, DNC, SCH

## Notes

**Profiles are per-character.** Settings are saved to `config\jobswap\CharName.xml` under your Ashita install directory. Each character maintains their own independent profile list.

**Duplicate job protection.** If a swap would result in main and sub being the same job, sub is automatically corrected to WAR. If main is WAR, sub defaults to MNK instead.

**Location validation.** If a job swap is attempted in a zone that does not permit it, the plugin will detect the server rejection after ~1.5 seconds and resend your original jobs. This corrects any gear-swap side effects caused by plugins like LuAshitacast reacting to the outgoing packet before the server rejects it.

## Settings File

Profiles are stored as XML and can be edited manually if needed:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<Jobswap>
    <Profiles>
        <Profile name="default" main="15" sub="3" />
        <Profile name="leveling" main="4" sub="20" />
    </Profiles>
</Jobswap>
```

Job IDs correspond to their standard FFXI numeric values (WAR=1, MNK=2, WHM=3, etc.).
