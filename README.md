# CS2 External Base
This is a external base for a kernel-mode cheat for CS2 (but it can be used for any game with a few tweaks). It is written in C++ and achieves memory reading and writing through a kernel-mode driver. The communication between the user-mode application and the kernel-mode driver is done through *Function Hooking*.

This project was created in order to learn more about game hacking and reverse engineering. It is meant to be used for educational purposes only, because of that offsets are definitely outdated. Please note that this project is for educational purposes only and should not be used for malicious activities.

Note: This is just a base, it is not a fully functional cheat. You will need to make your own driver and find the offsets yourself, if you want to use this project.

### Disclaimer
This project is for educational purposes only. I do not condone cheating in any form.

## Table of Contents
- [Features](#features)
- [Dependencies](#dependencies)
- [Get Started (Windows Only)](#get-started-windows-only)
- [Usage](#usage)
- [License](#license)
- [Contributing](#contributing)

## Features
- ESP (Name, Health, Box, Team Check)
- Aimbot (via entity origin, not bone)
- Bunny Hop
- Get process module base address through the kernel
- Stream Proof

## Dependencies
- [DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
- [CMake](https://cmake.org/)
- [GLM](https://github.com/g-truc/glm) and [ImGui](https://github.com/ocornut/imgui) (included as git submodules)

## Get Started (Windows Only)
1. Clone the repository recursively
```bash
git clone --recursive git@github.com:1neskk/CS2-External.git
```
2. Run the script `setup.bat` to build the project
3. Open the solution file inside the `build` directory
4. Set the start up project to `CS2` and build the solution
5. Run the resulting executable

## Usage
- Press 'INSERT' to toggle the imgui menu
- The 'Attach' button will attach the program to the game's proccess (the game must be open)
- When attached to the game's proccess the 'Detach' button will detach the program from the game's proccess
- The 'Exit' button will close the program

## License
This project is licensed under the Apache 2.0 License - see the [LICENSE](LICENSE) file for details

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.
