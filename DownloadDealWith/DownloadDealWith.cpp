#define PACKFOLDER "Game" // 游戏文件夹（发行目录下的）
// #define URI "" // download文件的uri，打包出来的download文件夹直接移动到服务器上

#define PACKASBUILDER // 保留此行编译为打包程序，注释此行编译为下载程序

#include <iostream>
#include <windows.h>
using namespace std;

typedef string(__stdcall* MD5)(string);
typedef void(__stdcall* BUILD)(string);

int main()
{
    HMODULE filebuild = LoadLibrary(L"FileBuild.dll");
    if (filebuild == NULL)
    {
        cout << "没找到 FileBuild.dll";
        return 0;
    }
    MD5 md5 = (MD5)GetProcAddress(filebuild, "CPP_GetMD5");
    BUILD build = (BUILD)GetProcAddress(filebuild, "CPP_BuildFile");

    // md5 计算
    // std::cout << md5("Hello World!") << endl;
    cout << "Example program start" << endl;
    cout << "This program is ured for uploading and downloading software packages to and from the server" << endl;
    cout << "The software package uses 7z.exe for volume compression, and this program supports users to pause downloads" << endl;
    cout << "Please refer to Readme.md" << endl;
    
#ifdef PACKASBUILDER
    build(PACKFOLDER);
#endif

#ifndef PACKASBUILDER
    cout << "No download examples available at the moment" << endl;
#endif

    cout << "Jump out from dlls" << endl;
    cout << "Example program end" << endl;
    system("pause");
    return 0;
}

