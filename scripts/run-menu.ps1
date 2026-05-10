param(
    [string]$Storage = "data/tasks.txt",
    [string]$BuildDir = "build"
)

$exe = Join-Path $BuildDir "task_manager.exe"
if (-not (Test-Path $exe)) {
    Write-Error "Executable not found. Run scripts\build.ps1 first."
    exit 1
}

& $exe --storage $Storage menu
exit $LASTEXITCODE
