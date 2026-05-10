# Setup script for Windows: installs CMake via winget if available
# Run as normal user; winget may require user acceptance for packages.

function Check-Command($name) {
    $null -ne (Get-Command $name -ErrorAction SilentlyContinue)
}

if (Check-Command cmake) {
    Write-Host "cmake is already installed."
    exit 0
}

Write-Host "cmake not found. Attempting to install with winget..."

if (-not (Check-Command winget)) {
    Write-Host "winget not found. Please install CMake manually from https://cmake.org/download/ or install winget."
    exit 1
}

Write-Host "Installing CMake via winget (requires internet and possible elevation)..."
winget install --id Kitware.CMake -e --silent

if (Check-Command cmake) {
    Write-Host "cmake installed successfully."
    exit 0
} else {
    Write-Host "Automatic install failed. Please install CMake manually: https://cmake.org/download/"
    exit 1
}
