@echo off
setlocal enabledelayedexpansion

:print_header
echo ------------------------------------------
echo      Graphics Converter - Build Script
echo ------------------------------------------
echo.

:print_step
echo [Step %~1] %~2

:clean_vendor
if "%~1"=="clean_vendor" (
    call :print_step "Clean" "Removing vendor directory"
    if exist vendor (
        rmdir /s /q vendor
        echo Vendor directory removed successfully
    ) else (
        echo Vendor directory not found
    )
    exit /b
)

:clean_build
if "%~1"=="clean_build" (
    call :print_step "Clean" "Removing build directory"
    if exist build (
        rmdir /s /q build
        echo Build directory removed successfully
    ) else (
        echo Build directory not found
    )
    exit /b
)

:build_project
call :print_step 1 "Configuring CMake project"
cmake -S . -B build
if %errorlevel% neq 0 (
    echo Configuration failed
    exit /b 1
)
echo Configuration successful

call :print_step 2 "Building project"
cmake --build build
if %errorlevel% neq 0 (
    echo Build failed
    exit /b 1
)
echo Build successful

echo.
echo Build process completed
exit /b

:print_usage
echo Usage: %0 [OPTION]
echo Options:
echo   --clean-vendor  Remove the vendor directory
echo   --clean-build   Remove the build directory
echo   --clean-all     Remove both vendor and build directories
echo   --help          Display this help message
echo If no option is provided, the script will build the project.
exit /b

:main
call :print_header

if "%~1"=="--clean-vendor" (
    call :clean_vendor
) else if "%~1"=="--clean-build" (
    call :clean_build
) else if "%~1"=="--clean-all" (
    call :clean_vendor
    call :clean_build
) else if "%~1"=="--help" (
    call :print_usage
) else if "%~1"=="" (
    call :build_project
) else (
    echo Invalid option: %~1
    call :print_usage
    exit /b 1
)

endlocal