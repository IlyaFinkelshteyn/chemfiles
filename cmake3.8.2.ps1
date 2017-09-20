$cmakeUninstallPath = "${env:ProgramFiles(x86)}\CMake\Uninstall.exe"
if([IO.File]::Exists($cmakeUninstallPath)) {
    Write-Host "Uninstalling previous CMake ..." -ForegroundColor Cyan
    # uninstall existent
    "`"$cmakeUninstallPath`" /S" | out-file ".\uninstall-cmake.cmd" -Encoding ASCII
    & .\uninstall-cmake.cmd
    del .\uninstall-cmake.cmd
    Start-Sleep -s 10
}

Write-Host "Installing CMake 3.8.2 ..." -ForegroundColor Cyan
$msiPath = "$($env:TEMP)\cmake-3.8.2-win32-x86.msi"

Write-Host "Downloading..."
(New-Object Net.WebClient).DownloadFile('https://cmake.org/files/v3.8/cmake-3.8.2-win32-x86.msi', $msiPath)

Write-Host "Installing..."
cmd /c start /wait msiexec /i $msiPath /quiet
del $msiPath

Add-Path 'C:\Program Files (x86)\CMake\bin'

remove-path 'C:\ProgramData\chocolatey\bin'
add-path 'C:\ProgramData\chocolatey\bin'

Write-Host "CMake 3.8.2 installed" -ForegroundColor Green
