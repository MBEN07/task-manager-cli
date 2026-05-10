# Build with CMake
param(
    [string]$BuildDir = "build"
)

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Error "cmake not found. Run scripts\setup-windows.ps1 first or install CMake manually."
    exit 1
}

cmake -S . -B $BuildDir
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

cmake --build $BuildDir
exit $LASTEXITCODE
