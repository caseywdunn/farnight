# farnight

Resources we drew on for this project:

- https://www.youtube.com/playlist?list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-, which follows https://learnopengl.com/ . The code we forked is at https://github.com/VictorGordan/opengl-tutorials .

- Character controller / camera based on https://www.3dgep.com/understanding-the-view-matrix/#fps-camera


## Building

    cd farnight/
    mkdir build
    cd build 
    cmake -G "MinGW Makefiles" .. # On Windows
    cmake --build .