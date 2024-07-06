#!/bin/bash

BLUE='\033[0;34m'
CYAN='\033[0;36m'
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_header() {
    echo -e "${BLUE}"
    echo "------------------------------------------"
    echo "     Graphics Converter - Build Script"
    echo "------------------------------------------"
    echo -e "${NC}"
}

print_step() {
    echo -e "${CYAN}[Step $1]${NC} $2"
}

clean_vendor() {
    print_step "Clean" "Removing vendor directory"
    if rm -rf vendor; then
        echo -e "${GREEN}Vendor directory removed successfully${NC}"
    else
        echo -e "${RED}Failed to remove vendor directory${NC}"
    fi
}

clean_build() {
    print_step "Clean" "Removing build directory"
    if rm -rf build; then
        echo -e "${GREEN}Build directory removed successfully${NC}"
    else
        echo -e "${RED}Failed to remove build directory${NC}"
    fi
}

build_project() {
    print_step 1 "Configuring CMake project"
    if cmake -S . -B build; then
        echo -e "${GREEN}Configuration successful${NC}"
    else
        echo -e "${RED}Configuration failed${NC}"
        exit 1
    fi

    print_step 2 "Building project"
    if cmake --build build; then
        echo -e "${GREEN}Build successful${NC}"
    else
        echo -e "${RED}Build failed${NC}"
        exit 1
    fi

    echo -e "\n${YELLOW}Build process completed${NC}"
}

print_usage() {
    echo "Usage: $0 [OPTION]"
    echo "Options:"
    echo "  --clean-vendor  Remove the vendor directory"
    echo "  --clean-build   Remove the build directory"
    echo "  --clean-all     Remove both vendor and build directories"
    echo "  --help          Display this help message"
    echo "If no option is provided, the script will build the project."
}

print_header

case "$1" in
--clean-vendor)
    clean_vendor
    ;;
--clean-build)
    clean_build
    ;;
--clean-all)
    clean_vendor
    clean_build
    ;;
--help)
    print_usage
    ;;
"")
    build_project
    ;;
*)
    echo -e "${RED}Invalid option: $1${NC}"
    print_usage
    exit 1
    ;;
esac
