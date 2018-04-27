$scriptpath = $MyInvocation.MyCommand.Path
$dir = Split-Path $scriptpath
Push-Location $dir
[Environment]::CurrentDirectory = $dir

[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

Write-Host "Setup directories"
New-Item "..\tmp" -itemtype directory -ErrorAction SilentlyContinue
New-Item "..\tools" -itemtype directory -ErrorAction SilentlyContinue
New-Item "..\tools\bin" -itemtype directory -ErrorAction SilentlyContinue

#intelseapi
Write-Host "Installing IntelSEAPI"
$repo = "intel/IntelSEAPI"
$exename = "IntelSEAPI-Windows.exe"
$destinationdir = "..\tmp"
$releases = "https://api.github.com/repos/$repo/releases"
$url = (Invoke-WebRequest $releases | ConvertFrom-Json)[0].assets[2].browser_download_url
$download = "https://github.com/$repo/releases/download/$tag/$exename"
Write-Host $download
Remove-Item $destinationdir\$exename -Force -ErrorAction SilentlyContinue 
Invoke-WebRequest $url -Out $destinationdir\$exename
& "$destinationdir\$exename" /S  /D=$dir\..\packages\IntelSEAPI-Windows

#vswhere
Write-Host "Installing VSWhere"
$repo = "Microsoft/vswhere"
$exename = "vswhere.exe"
$destinationdir = "..\tools\bin\vswhere"
$releases = "https://api.github.com/repos/$repo/releases"
$url = (Invoke-WebRequest $releases | ConvertFrom-Json)[0].assets[0].browser_download_url
$download = "https://github.com/$repo/releases/download/$tag/$exename"
New-Item $destinationdir -itemtype directory -ErrorAction SilentlyContinue 
Write-Host $download
Remove-Item $destinationdir\$exename -Force -ErrorAction SilentlyContinue 
Invoke-WebRequest $url -Out $destinationdir\$exename

# Nuget
Write-Host "Installing Nuget"
New-Item ..\tools\bin\nuget -itemtype directory -ErrorAction SilentlyContinue 
Invoke-WebRequest -Uri "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe" -OutFile "..\tools\bin\nuget\nuget.exe"
Write-Host "Nuget Deployed"

#Premake
Write-Host "Installing Premake"
$repo = "premake/premake-core"
$file = "premake.zip"
$exename = "premake5.exe"
$destinationdir = "..\tools\bin\premake"
$releases = "https://api.github.com/repos/$repo/releases"
$tag = (Invoke-WebRequest $releases | ConvertFrom-Json)[0].tag_name.Substring(1)
$name = $file.Split(".")[0]
$zip = "$name-$tag-windows.zip"
$dir = "$name-$tag"
$zipfile = "..\tmp\$name-$tag-windows.zip"
$download = "https://github.com/$repo/releases/download/v$tag/$zip"
New-Item $destinationdir -itemtype directory -ErrorAction SilentlyContinue
Write-Host $download
Invoke-WebRequest $download -Out $zipfile
Expand-Archive $zipfile -Force -DestinationPath "..\tmp"
Remove-Item $destinationdir\$exename -Force -ErrorAction SilentlyContinue 
Move-Item ..\tmp\$exename -Destination $destinationdir\$exename -Force
Remove-Item $zipfile -Force

#Poco
& "..\tools\bin\nuget\nuget.exe" install finsec.poco-windows-v140 -OutputDirectory ..\packages
& "..\tools\bin\nuget\nuget.exe" install finsec.poco-windows-v141 -OutputDirectory ..\packages

#gTest
& "..\tools\bin\nuget\nuget.exe" install gtest-vc140-static-64 -OutputDirectory ..\packages
& "..\tools\bin\nuget\nuget.exe" install Microsoft.googletest.v140.windesktop.msvcstl.dyn.rt-dyn -OutputDirectory ..\packages

Pop-Location
[Environment]::CurrentDirectory = $PWD

