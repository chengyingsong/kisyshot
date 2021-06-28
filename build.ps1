if (!(Test-Path "build")) {
    # create tool folder
    New-Item -ItemType Directory build
}
if (!(Test-Path "build/vswhere.exe")) {
    # download latest vswhere release from github
    $repo = "microsoft/vswhere"
    $file = "vswhere.exe"

    $releases = "https://api.github.com/repos/$repo/releases"

    Write-Host "get latest vswhere release"
    [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
    $tag = (Invoke-WebRequest -Uri $releases -UseBasicParsing | ConvertFrom-Json)[0].tag_name

    $download = "https://github.com/$repo/releases/download/$tag/$file"

    Write-Host "downloading latest vswhere release"

    [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
    Invoke-WebRequest $download -Out "build\$file"
}

if (Get-Command "cmake.exe" -ErrorAction SilentlyContinue) { 
    $installationPath = .\build\vswhere.exe -prerelease -latest -property installationPath
    if ($installationPath -and (Test-Path "$installationPath\Common7\Tools\vsdevcmd.bat")) {
        & "${env:COMSPEC}" /s /c "`"$installationPath\Common7\Tools\vsdevcmd.bat`" -no_logo && set" | foreach-object {
            $name, $value = $_ -split '=', 2
            Set-Content env:\"$name" $value
        }
        if (!(Test-Path "build")) {
            New-Item -ItemType Directory build
        }
        Set-Location build
        cmake.exe .. -G"NMake Makefiles"
        nmake.exe
        Set-Location ..
    }
    else {
        Write-Host "MSVC is not found, please install MSVC to build the executable"
    }
}else {
    Write-Host "cmake.exe is not found, please install CMakes to build the executable"
}
