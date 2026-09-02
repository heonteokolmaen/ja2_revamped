#ifndef __PROFILE_CONSTANTS_H
#define __PROFILE_CONSTANTS_H

// Split out of "soldier profile type.h" so ProfileID (Soldier Control.h) can see
// NUM_PROFILES without a circular include - "soldier profile type.h" includes
// "Soldier Control.h", so the dependency can't run the other way.

// Phase 4: 255 -> 2048. Gated by PROFILE_2048_EXPANSION (Ja2/GameVersion.h) the
// same way NUM_PROFILES_v111 gates the 170 -> 255 change below: saves written
// before that version only have NUM_PROFILES_v255 profiles on disk, and any
// code reading old save data must bound itself by the value that was actually
// in effect when that save was written, not by whatever NUM_PROFILES means today.
#define NUM_PROFILES		2048
#define NUM_PROFILES_v255	255
#define NUM_PROFILES_v111	170

// PHASE 6 CHECKLIST. Real memory check done: gMercProfiles' own
// usDynamicOpinionFlagmask[NUM_PROFILES][6] + sDynamicOpinionLongTerm[NUM_PROFILES]
// (soldier profile type.h) go from ~1.5MB total to ~100MB total at 2048 - trivial
// on any modern machine, no action needed there, just wanted a real number
// instead of a guess.
//
// DONE (PHASE6_PROFILE_FIELDS_WIDEN, GameVersion.h):
//   2. Tactical/Vehicles.h ubProfileID (vehicle passenger/owner) - widened
//      UINT8 -> ProfileID. Frozen _OLD_VEHICLETYPE_PHASE6 + version-gated
//      Load in Vehicles.cpp handles old saves.
//   3. Strategic/Strategic Movement.h PLAYERGROUP::ubProfileID - widened
//      UINT8 -> ProfileID. Already saved/loaded via a full UINT32
//      intermediate (Strategic Movement.cpp), so zero save-format risk.
//   4. Tactical/soldier profile type.h bBuddy[5]/bHated[5] and
//      bLearnToLike/bLearnToHate - these are UINT8 in the live struct (an
//      earlier note here wrongly said INT8/127 - that INT8 tier belongs to
//      the frozen OLD_MERCPROFILESTRUCT_101/_OLD_MERCPROFILESTRUCT legacy
//      structs, not the live one). Widened UINT8 -> ProfileID; Load()
//      version-gated field-by-field (Ja2/SaveLoadGame.cpp) since Save()
//      already wrote these via a raw blob read back field-by-field, not a
//      second blob read. Downstream narrow locals that stored/compared
//      these values (WhichBuddy/WhichHated params, bMercID/bBuddyID/
//      ubOtherProfileID locals in Soldier Profile.cpp, Merc Contract.cpp,
//      Strategic Merc Handler.cpp, AimMembers.cpp) widened to ProfileID too
//      - they were silently narrowing pSoldier->ubProfile at the call
//      boundary before this. Also fixed a real regression from Phase 4: several
//      "== NUM_PROFILES" sentinel checks (meaning "no buddy/hated", one past
//      the old 255 cap) broke silently when NUM_PROFILES became 2048 instead
//      of 255 - now compare against the frozen NUM_PROFILES_v255 instead
//      (Soldier Profile.cpp, Merc Contract.cpp, AimMembers.cpp,
//      LuaInitNPCs.cpp's l_CheckFact). IsMercOnTeam/FindSoldierByProfileID/
//      IsMercDead/CheckFact/SwapToProfile themselves stay UINT8 - each is its
//      own separate, wider migration, out of scope here; call sites cast
//      explicitly instead of relying on silent narrowing.
//   6. Tactical/soldier profile type.h MERCPROFILESTRUCT::bMercOpinion -
//      INT8 bMercOpinion[NUMBER_OF_OPINIONS]. NUMBER_OF_OPINIONS was a
//      hardcoded 255 (frozen as NUMBER_OF_OPINIONS_v255); now equals
//      NUM_PROFILES(2048), same recipe ENLARGED_OPINIONS(141) already used
//      once to grow it 75(NUMBER_OF_OPINIONS_OLD) -> 255. New save version
//      ENLARGED_OPINIONS_2048 (GameVersion.h) three-way-gates Load(): new
//      saves read the full (now 2048-wide) array; saves in
//      [ENLARGED_OPINIONS, ENLARGED_OPINIONS_2048) read NUMBER_OF_OPINIONS_v255
//      (255) entries into the front, rest stays zeroed by initialize(); older
//      saves keep reading NUMBER_OF_OPINIONS_OLD (75), unchanged. Save() needed
//      no change (raw blob write already reflects the live layout). Found and
//      fixed a real infinite-loop bug this widening exposed: two UINT8 loop
//      counters bounded by "< NUMBER_OF_OPINIONS" (XML_Opinions.cpp,
//      Soldier Profile.cpp's OverwriteMercOpinionsWithXMLData) could never
//      reach 2048 and would wrap forever - same loop-counter class Phase 4
//      audited for NUM_PROFILES, just missed here since NUMBER_OF_OPINIONS is
//      its own separate macro. TEMPPROFILETYPE::bMercOpinion (Soldier
//      Profile.h) already referenced the same NUMBER_OF_OPINIONS macro rather
//      than a hardcoded size, so it grew in lockstep automatically - no
//      type-mismatch fix needed there (unlike TEMPPROFILETYPE's bBuddy/bHated,
//      which were hardcoded UINT8[5] and did need item 4's element-by-element
//      conversion fix). Every other bMercOpinion[] access site was already
//      either OKToCheckOpinion()-gated (now correctly permits the full
//      2048 range with zero code change, since it reads the same macro) or
//      indexed by a UINT8/ProfileID that's inherently bounded within range.
//      Compile-and-link verified (real MSVC/Ninja build, JA2.exe produced).
//      Memory cost verified: bMercOpinion alone goes from ~522KB to ~4.19MB
//      total across gMercProfiles[2048] - trivial.
//
// NOT STARTED:
//   1. Strategic/Rebel Command.cpp - mercProfileId bit-packed to 8 bits in a
//      UINT32 wire format (Serialise/DeserialiseMission{First,Second}Event).
//      Widening this one isn't mechanical - it means reshuffling the bit
//      layout (mission/duration/extraBits share the other 24 bits).
//   5. Laptop/mercs.cpp:396 - the whole MERC-hiring-availability subsystem
//      (gubMercArray, AimMercArray, gConditionsForMercAvailability::uiIndex)
//      stores profile-ID-shaped values in its own UINT8 arrays/fields,
//      independent of ProfileID entirely. Deeper than the others - touching
//      it for real cascades like a mini Phase 2 scoped to this one file.
//
// PHASE 7 (regression pass) - DONE. Tested against a real pre-Phase-6 v1.13
// save (three SavedGames from an actual playthrough) plus a fresh new game,
// both through a local test gamedir (Data/ junctioned to a real v1.13
// install, save files copied - never the originals). Findings:
//   - New game -> save -> load round-trip: clean across all 2048 profile
//     slots, zero errors (LoadSavedMercProfiles, verified via temporary
//     per-profile position/checksum instrumentation, since removed).
//   - Sector entry / .map loading: confirmed via real engine log output
//     ("A9 ENTER SECTOR TIME: 1.58 SECONDS", "VICKI HAS ARRIVED").
//   - Time advancement, squad travel, Laptop UI: all working through the
//     normal IMP-creation new-game flow.
//   - Old-save loading (the real save above) surfaced and got four genuine,
//     confirmed bugs fixed (all in Ja2/SaveLoadGame.cpp's
//     MERCPROFILESTRUCT::Load): (a) the item-4 field-by-field old-version
//     reads not incrementing "buffer" to account for the UINT8->ProfileID
//     growth, failing the numBytesRead+buffer==SIZEOF_MERCPROFILESTRUCT_POD
//     check on every old-format load; (b) usDynamicOpinionFlagmask's
//     DRUG_SYSTEM_REDONE-tier read was gated on the wrong version constant
//     and read the LIVE (2048-wide) array size for saves that were actually
//     still NUM_PROFILES_v255-wide on disk - a real over-read, not just a
//     validation-check failure; (c)/(d) two out-of-bounds reads in that same
//     block's older tiers, reading past a NUM_PROFILES_v255-sized tmp array;
//     (e) the uiProfileChecksum/GetChecksum() hard-fail on a genuinely empty
//     (bLifeMax<=0) profile slot aborted the ENTIRE load instead of letting
//     the caller's own existing "restore placeholder data for an invalid
//     profile" fallback handle it - now only hard-fails when bLifeMax>0.
//     Verified byte-perfect (real file position, not just internal
//     bookkeeping) across 60+ consecutive profile slots of the real save.
//   - One specific old save has an unresolved anomaly at profile #61 (data
//     that looks like a genuine 0xFF-filled "never written" region rather
//     than the all-zero pattern every other empty slot had) - starts at the
//     exact correct file position (independently verified), so not a
//     position/reading bug on this session's part; most likely explained by
//     a version gap between this repo's exact snapshot and whatever build
//     actually wrote that save, not a Phase 6 regression. Not reproduced on
//     a fresh save, and not blocking - documented rather than chased further.
//   - Separately: the debug "New Game" quick-start path (JA2TESTVERSION
//     canned-squad shortcut) has its own narrow, pre-existing bug -
//     INTERFACE_CLOCK_X/Y (Strategic/Game Clock.cpp) are plain globals only
//     ever assigned in Tactical/Interface Panels.cpp, so a playthrough that
//     starts directly on the strategic map without ever visiting a tactical
//     screen first renders/registers the clock at (0,0), hidden behind the
//     merc info panel - can't advance time or reach a sector. Doesn't affect
//     the normal IMP-creation flow (confirmed working above); not something
//     this session's profile-widening work touched or introduced.

#endif
