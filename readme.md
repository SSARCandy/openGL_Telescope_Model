# Telescope Model

A complete telescope (with equatorial) simulator written in OpenGL.

![demo](https://raw.githubusercontent.com/SSARCandy/openGL_Telescope_Model/master/demo/demo.gif)



### Build Code

**Requirement**

- [cmake](https://cmake.org/) - build tools

**Linux**

```bash
$ sudo apt-get install build-essential g++ cmake libx11-dev freeglut3-dev libglu1-mesa-dev libxcb1-dev libxext-dev libxxf86vm-dev libxi-dev libxmu-dev glew-utils libglew1.5-dev libgl1-mesa-dev freeglut3-dev libglut-dev
$ git submodule update --init --recursive
$ ./build.sh
$ ./build/Telescope
```


### Usage

**Control**

Key      | Descriptions              | 中文
---------|---------------------------|-------------
`Mouse`    | Drag to change view       | (拖拉調整視角)                
`Z`, `X`     | Zoom in/out               | (調整遠近)                    
`A`, `D`     | Adjust Right ascension(RA)| (調整赤經)               
`W`, `S`     | Adjust Declination(Dec)   | (調整赤緯)              
`[`, `]`     | Adjust tripod             | (調整腳架張角)                
`+`, `-`     | Adjust Motor Speed        | (調整馬達速度)  
`G`        | GoTo SUN                  | (自動追蹤太陽)           
`P`        | Park                      | (歸位至初始位置)         
`C`        | Crazy Mode                | (瘋狂亂移動所有可動關節)
`ArrowKey` | SpotLight translation     | (點光源平移)                  
`PgUp`     | SpotLight Up              | (點光源向上)
`PgDown`   | SpotLight Down            | (點光源向下)


**Setting**

Key      | Descriptions              | 中文
---------|---------------------------|-------------
`L`      | Shading on/off       |(光源)  
`K`      | Antialias on/off     |(反鋸齒)
`J`      | PolygonOffset on/off |(實心)  
`H`      | Flashlight on/off    |(手電筒)
`Esc`    | Exit                 |(關閉程式)                    




### Sceenshots


![img](https://raw.githubusercontent.com/SSARCandy/openGL_Telescope_Model/master/demo/01.png)
![img](https://raw.githubusercontent.com/SSARCandy/openGL_Telescope_Model/master/demo/02.png)
![img](https://raw.githubusercontent.com/SSARCandy/openGL_Telescope_Model/master/demo/03.png)