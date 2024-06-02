# Chess Game with Pygame 

A simple chess game drag-n-drop UI built with Pygame. 

## Roadmap 
1. Make Chess Configuration UI 
    - Standard Chess Configuration [done!] 
    - Fisher's Configuration  

2. Make drag-n-drop the pieces [wip] 

3. Add AI commentary [future]

Planning on adding some GPT flair on this and give AI commentary. 

## Inter.C 

A simple raylib program to interpolate between chess board configurations. 

## Raylib installation (Linux/Ubuntu) 

1. Install git and make 
```
sudo apt install build-essential git
```

2. Install raylib deps
```
sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
```



3. Clone the source 
```
git clone https://github.com/raysan5/raylib.git raylib
cd raylib/src/
make PLATFORM=PLATFORM_DESKTOP # To make the static version
```

4. Install Raylib 
```
sudo make install 
```

5. 
```
 make && ./inter
```

### Inter.c Demo (Chess Position Smooth interpolation) 

https://github.com/oysters76/pychess/assets/75514064/a84c54a5-7d95-4aa6-98b9-050b07eab467


