<#
.SYNOPSIS
    Builds all X2Cscope library archives and packages them with the interface files for a release.

.DESCRIPTION
    1. Checks that the compilers pinned in the makefile are installed, and warns if the working tree
       has uncommitted changes or HEAD is not the release tag.
    2. Full rebuild with "make release" (clean + all). The makefile has no header dependencies, so an
       incremental build can reuse stale objects.
    3. Checks that the archive of every *.mk file (plus the combined PIC24/dsPIC33 archive) exists in
       dist/ and contains objects.
    4. Zips these archives and the tracked files of interface/ into
       dist/libx2cscope<major>-elf_<tag>.zip (e.g. libx2cscope3-elf_v3_2.zip).

    Upload the zip to the GitHub release page by hand.

.PARAMETER Tag
    Release tag, e.g. v3.3.

.PARAMETER SkipBuild
    Package the archives that are in dist/ without rebuilding them.

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File scripts\make_release.ps1 -Tag v3.3
#>
param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern('^v\d+\.\d+(\.\d+)?$')]
    [string]$Tag,

    [switch]$SkipBuild
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$combinedVar = 'COMBINED_LIB'

function Write-Step([string]$text) { Write-Host "`n== $text" -ForegroundColor Cyan }
function Write-Warn([string]$text) { Write-Host "WARNING: $text" -ForegroundColor Yellow }

function Get-MakeVariable([string]$text, [string]$name) {
    $match = [regex]::Match($text, "(?m)^$name\s*:=\s*(\S+)")
    if (-not $match.Success) { throw "$name not found in the makefile" }
    return $match.Groups[1].Value
}

Push-Location $root
$stage = Join-Path $env:TEMP ("x2cscope_release_" + [guid]::NewGuid().ToString('N'))
try {
    # ---- 1. Environment and working tree --------------------------------------------------------
    Write-Step 'Checks'
    $makefile = Get-Content -Raw makefile
    $xc32Version = Get-MakeVariable $makefile 'XC32_VERSION'
    $xcdscVersion = Get-MakeVariable $makefile 'XCDSC_VERSION'
    $outDir = (Get-MakeVariable $makefile 'LIB_OUT_DIR_PATH').TrimEnd('/', '\')
    $microchip = Join-Path $env:ProgramFiles 'Microchip'
    $compilers = @(
        (Join-Path $microchip "xc32\$xc32Version\bin\xc32-gcc.exe"),
        (Join-Path $microchip "xc-dsc\$xcdscVersion\bin\xc-dsc-gcc.exe")
    )
    foreach ($compiler in $compilers) {
        if (-not (Test-Path $compiler)) { throw "Compiler not found: $compiler (versions are pinned in the makefile)" }
        Write-Host "ok  $compiler"
    }
    $ar = Join-Path $microchip "xc32\$xc32Version\bin\xc32-ar.exe"
    $tar = Join-Path $env:SystemRoot 'System32\tar.exe'   # bsdtar, writes zip files with "/" paths
    if (-not (Test-Path $tar)) { throw "tar.exe not found: $tar" }
    if (-not $SkipBuild -and -not (Get-Command make -ErrorAction SilentlyContinue)) {
        throw 'make not found: add MPLAB X gnuBins\GnuWin32\bin to PATH (see README)'
    }

    $dirty = git status --porcelain -- X2CScope interface makefile '*.mk'
    if ($dirty) { Write-Warn "uncommitted changes in the sources:`n$($dirty -join "`n")" }
    $head = git rev-parse HEAD
    $tagCommit = git rev-parse -q --verify "refs/tags/$Tag^{commit}"
    if (-not $tagCommit) {
        Write-Warn "tag $Tag does not exist yet; building $(git rev-parse --short HEAD) ($(git rev-parse --abbrev-ref HEAD))"
    } elseif ($tagCommit -ne $head) {
        Write-Warn "HEAD $(git rev-parse --short HEAD) is not tag $Tag ($(git rev-parse --short $tagCommit))"
    }

    # ---- 2. Full rebuild -----------------------------------------------------------------------
    if ($SkipBuild) {
        Write-Warn 'build skipped: packaging the archives that are in dist/'
    } else {
        Write-Step 'Full rebuild (make release)'
        & make release
        if ($LASTEXITCODE -ne 0) { throw "make release failed (exit code $LASTEXITCODE)" }
    }

    # ---- 3. Check the archives -----------------------------------------------------------------
    Write-Step 'Archives'
    $expected = @(Get-ChildItem -Filter '*.mk' | ForEach-Object { Get-MakeVariable (Get-Content -Raw $_.FullName) 'TARGET_FILENAME' })
    $expected += Get-MakeVariable $makefile $combinedVar
    $expected = $expected | Sort-Object -Unique
    $missing = @()
    foreach ($lib in $expected) {
        $path = Join-Path $outDir $lib
        $objects = 0
        if (Test-Path $path) { $objects = @(& $ar t $path | Where-Object { $_ -like '*.o' }).Count }
        if ($objects -gt 0) { Write-Host ("ok  {0} ({1} objects)" -f $lib, $objects) } else { $missing += $lib }
    }
    if ($missing) { throw "missing or empty archives in ${outDir}/:`n$($missing -join "`n")" }
    $extra = @(Get-ChildItem -Path $outDir -Filter '*.a' | Where-Object { $expected -notcontains $_.Name })
    if ($extra) { Write-Warn "not packaged (no *.mk target): $($extra.Name -join ', ')" }

    # ---- 4. Zip --------------------------------------------------------------------------------
    Write-Step 'Zip'
    $major = $Tag.TrimStart('v').Split('.')[0]
    $zipName = "libx2cscope$major-elf_$($Tag.Replace('.', '_')).zip"
    $zipPath = Join-Path (Resolve-Path $outDir) $zipName

    New-Item -ItemType Directory -Path $stage | Out-Null
    foreach ($lib in $expected) { Copy-Item (Join-Path $outDir $lib) $stage }
    $interfaceFiles = @(git ls-files interface)   # tracked files only, no local copies or backups
    if (-not $interfaceFiles) { throw 'no tracked files in interface/' }
    foreach ($file in $interfaceFiles) {
        $target = Join-Path $stage $file
        New-Item -ItemType Directory -Force -Path (Split-Path -Parent $target) | Out-Null
        Copy-Item $file $target
    }

    if (Test-Path $zipPath) { Remove-Item $zipPath }
    & $tar -a -c -f $zipPath -C $stage @($expected + 'interface')
    if ($LASTEXITCODE -ne 0) { throw "tar failed (exit code $LASTEXITCODE)" }

    $entries = @(& $tar -t -f $zipPath | Where-Object { -not $_.EndsWith('/') })
    $entries | ForEach-Object { Write-Host "    $_" }
    if ($entries.Count -ne $expected.Count + $interfaceFiles.Count) {
        throw "zip has $($entries.Count) files, expected $($expected.Count + $interfaceFiles.Count)"
    }

    Write-Step 'Done'
    Write-Host ("{0} ({1} archives, {2} interface files, {3:N0} kB)" -f $zipPath, $expected.Count, $interfaceFiles.Count, ((Get-Item $zipPath).Length / 1KB))
    Write-Host "Upload it to the release page of $Tag."
}
finally {
    if (Test-Path $stage) { Remove-Item -Recurse -Force $stage }
    Pop-Location
}
