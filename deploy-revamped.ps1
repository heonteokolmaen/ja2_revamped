# Rebuilds JA2 Revamped and deploys it to the real install folder.
#
# What this does:
#   1. Builds the JA2 target (msvc-debug preset) into gamedir\, the repo's
#      isolated local test environment.
#   2. Copies gamedir\ into C:\Games\Jagged Alliance 2 Revamped\, EXCLUDING
#      gamedir\Data (a junction to a real v1.13 install's vanilla assets -
#      the target already has its own real copy of that, never touched here).
#
# Test in gamedir first before trusting a build enough to redeploy - this
# script does NOT skip that step, it just automates the "I'm happy with it,
# push it out" part. Re-running after the first deploy is fast: robocopy
# only copies files that actually changed.
#
# Usage:
#   powershell -File deploy-revamped.ps1              # build + deploy
#   powershell -File deploy-revamped.ps1 -SkipBuild    # deploy only (gamedir already built)

param(
	[switch]$SkipBuild
)

$ErrorActionPreference = "Stop"

$repoRoot = $PSScriptRoot
$gamedir  = Join-Path $repoRoot "gamedir"
$target   = "C:\Games\Jagged Alliance 2 Revamped"
$cmake    = "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
$vcvars   = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"

if (-not $SkipBuild) {
	Write-Output "=== Building JA2 (msvc-debug) ==="
	cmd /c "`"$vcvars`" && `"$cmake`" --build --preset msvc-debug --target JA2 -- -k0"
	if ($LASTEXITCODE -ne 0) {
		Write-Error "Build failed (exit $LASTEXITCODE) - not deploying a broken build. Fix errors and re-run."
	}
}

Write-Output "=== Deploying $gamedir -> $target (excluding Data\) ==="
robocopy $gamedir $target /E /XD "$gamedir\Data" /XJ /R:1 /W:1 /NFL /NDL /NP /TEE

# Robocopy exit codes are a bitmask, not 0/1 - anything 0-7 is success.
# See: https://learn.microsoft.com/windows-server/administration/windows-commands/robocopy
if ($LASTEXITCODE -ge 8) {
	Write-Error "Robocopy reported real failures (exit $LASTEXITCODE) - check output above."
} else {
	Write-Output "=== Deploy complete (robocopy exit $LASTEXITCODE - success) ==="
}
