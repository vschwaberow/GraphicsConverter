# gfxconverter

A modern, cross-platform tool for converting images to different retro graphic formats.

## Features

- Convert modern image formats to C64 Koala format
- Real-time preview of conversion results
- Advanced dithering options
- Color reduction algorithms optimized for C64 palette
- User-friendly GUI built with Dear ImGui
- Cross-platform support (Windows, macOS, Linux)

## Building from Source

### Prerequisites

- CMake 3.14 or higher
- C++20 compatible compiler
- OpenGL

### Dependencies

The following dependencies are automatically fetched and built by CMake:

- Dear ImGui
- GLFW
- stb
- Native File Dialog
- spdlog

### Build Instructions

1. Clone the repository:
```bash
git clone https://github.com/vschwaberow/GraphicsConverter.git
cd GraphicsConverter
```

2. Use the build scripts to compile the project:
```bash
./compile.sh
```

Or the PowerShell script on Windows:
```powershell
.\compile.ps1
```

Or manually with CMake:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage

1. Launch the application
2. Use the "Open" button to select an image file
3. Adjust conversion settings as needed
4. Preview the result in real-time
5. Save the converted image in C64 Koala format

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- The Commodore 64 community for keeping the platform alive
- Contributors to the open-source libraries used in this project

## Contact

For questions and support, please open an issue on the GitHub repository.