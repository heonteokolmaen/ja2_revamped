
# JA2 Revamped

<br />
<br />
<br />

<p align="center">
  <img src="ja2v1.13.png" alt="JA2 v1.13">
</p>

<br />
<br />



### Preamble

JA2 Revamped is a personal modernization fork of [Jagged Alliance 2 v1.13](https://github.com/1dot13/source) — the long-running community mod for Jagged Alliance 2. It starts from the full v1.13 codebase and layers in structural fixes and quality-of-life improvements aimed at making the engine more robust under larger, more customized rosters and mods, plus an ongoing modernization effort (rendering pipeline, UI) beyond that.

All credit for the base v1.13 codebase goes to its original developers and contributors — see [The Bear's Pit Forum](https://thepit.ja-galaxy-forum.com), [the 1.13 starter documentation](https://github.com/1dot13/documentation), and [the 1.13 Discord](https://discord.gg/GqrVZUM) for anything general to v1.13 itself. This fork is where the Revamped-specific changes live.


### What's changed in Revamped

Work so far has focused on closing real structural limits in the engine rather than adding new content:

- **Mercenary profile system expanded from 255 to 2048 slots.** The hard 255-profile cap (`NUM_PROFILES`) that has capped custom rosters since the original game is gone — mercs, vehicles, and related profile-indexed data all support the wider range now, with old-save compatibility handled via version-gated loading rather than breaking existing saves.
- **AIM/MERC hiring subsystem widened to match.** Before this, the hiring-availability system (the code behind the AIM and M.E.R.C. websites) still silently capped at 255 independently of the profile system itself — any merc above #255 would get aliased to a low-numbered one instead of hired correctly. Fixed across both websites' full hiring path.
- **AIM Facial Index (mugshot browser) real pagination.** The visual merc-picker screen used to page through the roster via a hardcoded three-page cycle, capping its display at 120 mercs no matter how large the roster actually was. It now computes real page counts and cycles through all of them, in any language.
- **Rebel Command mission identity widened to the full profile range.** The Rebel Command feature's internal mission-tracking data only had room for an 8-bit merc identity, silently truncating anyone above #255 sent on a mission. Repartitioned to fit the full range without touching any other field, with a save-version gate so existing saves aren't misread.

See individual commits on the `Revamped-V1` branch for full technical detail on each of these — each one documents what changed, why, and what was verified.


### Installation

JA2 Revamped does not, and cannot, distribute Jagged Alliance 2's original game assets — those are commercial property. You need your own legitimate copy of Jagged Alliance 2 either way.

1. Install the original Jagged Alliance 2.
2. Build JA2 Revamped from source (see below) and point it at your install's `Data` folder.
3. Modify ini settings if you like.
4. Play the game.

   If you face issues with higher resolutions, alt+tab not working, blackscreen, etc.,
   run the `cnc-ddraw-config.exe` in the game folder and adjust settings to your liking
   (those issues can occur due to the combination of an old game and modern OS/hardware —
   `cnc-ddraw` helps avoid those).

   Downloadable, pre-built Windows releases are planned for this fork but not yet published —
   for now, building from source is the way to run it. Check the
   [releases page](https://github.com/heonteokolmaen/ja2_revamped/releases) for updates.


### Visual Studio setup

1. Run `Visual Studio 2019` or newer.
2. Clone and open the location with the source code using one of these two options:
    * Click `Clone a repository`
        * Enter `git@github.com:heonteokolmaen/ja2_revamped.git` or `https://github.com/heonteokolmaen/ja2_revamped.git` in the Repository location field, select the path you want to clone the repository to and click `Clone`.
        * Double-click on `Folder View` in the `Solution Explorer`
    * Click `Open a local folder`
        * Use this option if you already cloned the repository yourself.
3. Visual Studio detects `CMakePresets.json` and lists the `msvc-*` configurations. Executables are written to `gamedir/`.
4. To build only some applications, or to debug against a different JA2 1.13 install, add/modify a `CMakeUserPresets.json` in the source root that inherits one of the shipped presets and sets the `Applications` and/or `CMAKE_RUNTIME_OUTPUT_DIRECTORY` `cacheVariables`. Language is chosen at runtime, not at configure time.
5. Use `Build -> Build All` to build.


### Reports

For anything specific to Revamped's own changes, open an issue on this repository.

For anything general to v1.13 itself (the base this fork builds on), visit [Bug reports at Bear's Pit Forum](http://thepit.ja-galaxy-forum.com/index.php?t=thread&frm_id=216&) or join the [Bear's Pit Discord](https://discord.gg/GqrVZUM "Bear's Pit Discord").


### Participation

This is currently a personal project. Feel free to open issues or discuss on GitHub either way.
