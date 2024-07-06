$BLUE = "`e[0;34m"
$CYAN = "`e[0;36m"
$GREEN = "`e[0;32m"
$RED = "`e[0;31m"
$YELLOW = "`e[1;33m"
$NC = "`e[0m"

function Print-Header {
    Write-Host "$BLUE------------------------------------------"
    Write-Host "     Graphics Converter - Build Script"
    Write-Host "------------------------------------------$NC"
}

function Print-Step {
    param($stepNum, $message)
    Write-Host "${CYAN}[Step $stepNum]${NC} $message"
}

function Clean-Vendor {
    Print-Step "Clean" "Removing vendor directory"
    if (Test-Path vendor) {
        Remove-Item -Recurse -Force vendor
        Write-Host "${GREEN}Vendor directory removed successfully${NC}"
    } else {
        Write-Host "${RED}Vendor directory not found${NC}"
    }
}

function Clean-Build {
    Print-Step "Clean" "Removing build directory"
    if (Test-Path build) {
        Remove-Item -Recurse -Force build
        Write-Host "${GREEN}Build directory removed successfully${NC}"
    } else {
        Write-Host "${RED}Build directory not found${NC}"
    }
}

function Build-Project {
    Print-Step 1 "Configuring CMake project"
    if (cmake -S . -B build) {
        Write-Host "${GREEN}Configuration successful${NC}"
    } else {
        Write-Host "${RED}Configuration failed${NC}"
        exit 1
    }

    Print-Step 2 "Building project"
    if (cmake --build build) {
        Write-Host "${GREEN}Build successful${NC}"
    } else {
        Write-Host "${RED}Build failed${NC}"
        exit 1
    }

    Write-Host "`n${YELLOW}Build process completed${NC}"
}

function Print-Usage {
    Write-Host "Usage: .\build.ps1 [OPTION]"
    Write-Host "Options:"
    Write-Host "  -CleanVendor  Remove the vendor directory"
    Write-Host "  -CleanBuild   Remove the build directory"
    Write-Host "  -CleanAll     Remove both vendor and build directories"
    Write-Host "  -Help         Display this help message"
    Write-Host "If no option is provided, the script will build the project."
}

Print-Header

switch ($args[0]) {
    "-CleanVendor" { Clean-Vendor }
    "-CleanBuild" { Clean-Build }
    "-CleanAll" { 
        Clean-Vendor
        Clean-Build 
    }
    "-Help" { Print-Usage }
    $null { Build-Project }
    default { 
        Write-Host "${RED}Invalid option: $($args[0])${NC}"
        Print-Usage
        exit 1
    }
}