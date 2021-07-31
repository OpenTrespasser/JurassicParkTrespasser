$env:PATH = "$env:PATH;.\BuildTools\CMake\bin"
$archivedir = New-Item -ItemType Directory -Force -Path .\Archive
$builddir = New-Item -ItemType Directory -Force -Path .\Build\CI
$basedir = Get-Location

$configurations = @("Debug", "Release", "Final")

function Generate-Solution(){
    .\BuildTools\CMake\bin\cmake.exe -G "Visual Studio 16 2019" -A Win32 -S . -B $builddir -D USE_TRESPASSER_DIRECTORY=false
}

function Build-Solution([string] $config)
{
    Write-Host "Begin building configuration $config"

    Push-Location $builddir
    cmake --build . --config $config
    ctest -C $config --verbose
    $testresultdest = New-Item -ItemType Directory -Force -Path $archivedir\test\$config
    foreach($resultfile in (Get-ChildItem -Path . -Filter *_TestResults.xml -File -Recurse)){
        Copy-Item -Path $resultfile.FullName -Destination $testresultdest
    }
    Pop-Location
}


function Bundle-Demo([string] $config){
    Push-Location $builddir
    $version = (Get-Item .\cmake\trespass\$config\OpenTrespasser.exe).VersionInfo.ProductVersion
    $packagename = "OTP.$version.$config"
    $packagefile = "$packagename.zip"
    
    cpack -G ZIP -C $config -D CPACK_PACKAGE_FILE_NAME=$packagename --verbose
    Move-Item -Path $packagefile -Destination $archivedir\$packagefile -Force
    Pop-Location
}

function Bundle-Debuginfo(){
    Write-Host "Bundling debug info"

    Push-Location $builddir
    $tempdir = New-Item -ItemType Directory -Force -Path packaging\Debuginfo
    foreach ($config in $configurations) {
        Copy-Item -Recurse -Path .\cmake\trespass\$config\ -Filter *.pdb -Destination $tempdir -Force
        Copy-Item -Recurse -Path .\cmake\trespass\$config\ -Filter *.map -Destination $tempdir -Force
        Copy-Item -Recurse -Path .\cmake\trespass\$config\ -Filter *.exe -Destination $tempdir -Force
    }
    Compress-Archive -Path $tempdir\* -DestinationPath $archivedir\OTP-Debuginfo.zip -Force
    Pop-Location
}



Generate-Solution
foreach($config in $configurations){
    Build-Solution $config
    Bundle-Demo $config
}
Bundle-Debuginfo

Write-Host "Build completed"
