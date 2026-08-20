# Projeto_Raycasting_C_Topicos
Jogo pseudo 3d(Raycasting) utilizando o C com a biblioteca raylib.

# Como Rodar:
Windows:
Entre na pasta build e execute o main.exe.
Modificações:
No terminal entre na pasta build, e depois de salvo o arquivo execute o código "cmake --build ."
depois rode o main.exe.

Linux:
Exclua a pasta build.
execute os seguintes comandos no termainal:
conan profile detect --force
export CMAKE_POLICY_VERSION_MINIMUM=3.5 
conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .
Rode o código com o comando "./main" no terminal dentro da pasta build.

Modificações:
Salve o arquivo, e no terminal na pasta build rode o comando "cmake --build ." e depois rode o código com o comando "./main".
