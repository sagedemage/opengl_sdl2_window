# Download libraries Manually (Ubuntu 22.04.2 LTS)

## Download dependencies
### Go to these links
- https://github.com/libsdl-org/SDL
- https://github.com/libsdl-org/SDL_image/
- https://github.com/FluidSynth/fluidsynth
- https://github.com/libsdl-org/SDL_mixer

### Download the released source files
- SDL2-X.X.X.tar.gz - [SDL2]
- SDL2_image-X.X.X.tar.gz - [SDL2_image]
- SDL2_mixer-X.X.X.tar.gz - [SDL2_mixer]
- Source code (tar.gz) - [fluidsynth]

## Compile and Install Dependencies


### Install Dependencies for SDL2_mixer
```
sudo apt install libpulse-dev libasound2-dev
```

### Compile and Install fluidsynth for SDL2_mixer
```
cd fluidsynth-X.X.X
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
sudo cmake --install . --config Release
```

### Compile and Install SDL2
```
cd SDL-X.X.X
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
sudo cmake --install . --config Release
```

### Compile and Install SDL2_image
```
cd SDL_image-X.X.X
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
sudo cmake --install . --config Release
```

### Compile and Install SDL2_mixer
```
cd SDL_mixer-X.X.X
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
sudo cmake --install . --config Release
```

## Configure Linker
### Setup links and cache after installing libraries manually
```
sudo ldconfig
```
