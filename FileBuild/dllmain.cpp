// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

using namespace std;
namespace fs = std::filesystem;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C"
{
    // 这里其实是非C风格的函数
    __declspec(dllexport) std::string __stdcall CPP_GetMD5(std::string);
    __declspec(dllexport) void __stdcall CPP_BuildFile(std::string);
    __declspec(dllexport) bool __stdcall CPP_CheckFile(std::string, std::string);
    // 真正C风格函数
    __declspec(dllexport) bool __cdecl C_CheckFile(char*, char*);
    __declspec(dllexport) int* __cdecl C_WriteAsHash(char*);
    __declspec(dllexport) void __cdecl C_WriteInfo(int*);
}

string CPP_GetMD5(string str)
{
    return MD5(str).toStr();
}

class node
{
public:
    node(bool in_isFile, string in_name = "")
    {
        isFile = in_isFile; 
        name = in_name;
    }

    bool isFile;
    string name;
    vector<char>* data; // byte流数据

    node* parent; // 父节点
    vector<node*> child; // 子节点
};

bool FileExist(const std::string& filePath)
{
    // 打开文件
    std::ifstream file(filePath, std::ios::binary | std::ios::in);
    
    if (!file)
    {
        file.close();
        return false;
    }
    else
    {
        file.close();
        return true;
    }
}

std::vector<char>* File2Stream(const std::string& filePath) {
    // 打开文件
    std::ifstream file(filePath, std::ios::binary | std::ios::in);

    if (!file)
    {
        cout << "[FileBuild.dll::File2Stream] Cannot open file " + filePath << endl;
        system("pause");
    }

    // 获取文件大小
    std::streamsize fileStart = file.tellg();
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg() - fileStart;
    file.seekg(0, std::ios::beg);

    // 读取文件内容到 vector 中
    vector<char>* buffer = new vector<char>(fileSize);
    if (!file.read(buffer->data(), fileSize))
    {
        cout << "[FileBuild.dll::File2Stream] Cannot read file " + filePath << endl;
        system("pause");
    }

    file.close();
    return buffer;
}

void Stream2File(const string& filePath, vector<char>* buff)
{
    ofstream file(filePath, std::ios::binary | std::ios::out);
    if (!file)
    {
        cout << "[FileBuild.dll::Stream2File] Cannot open file " + filePath << endl;
        system("pause");
    }
    file.write(buff->data(), buff->size());
    file.close();
}

void TraverseFolder(const std::string& folderPath, node* n)
{
    for (const auto& entry : fs::directory_iterator(folderPath))
    {
        if (entry.is_directory()) {
            // 是文件夹
            node* c = new node(false, entry.path().string());
            c->parent = n;
            n->child.push_back(c);

            TraverseFolder(entry.path().string(), c);
        }
        else {
            // 是文件
            node* c = new node(true, entry.path().string());
            c->parent = n;
            n->child.push_back(c);
        }
    }
}

class FileTree
{
public:
    FileTree(string* rootbasicpath)
    {
        root = new node(false, *rootbasicpath);
        TraverseFolder(*rootbasicpath, root);
    }

    node* root;

    string pathinfo = "";

    void CmdShowNode(node* n)
    {
        if (n->isFile)
        {
            cout << "[FileBuild.dll][FileTree::CmdShowNode] File:" + n->name << endl;
        }
        else
        {
            cout << "[FileBuild.dll][FileTree::CmdShowNode] Folder:" + n->name << endl;
            for (int i = 0; i < n->child.size(); i++)
            {
                CmdShowNode(n->child[i]);
            }
        }
    }

    string GetNum(int i)
    {
        if (i < 10)
        {
            return "00" + to_string(i);
        }
        else if (i < 100)
        {
            return "0" + to_string(i);
        }
        else if (i < 1000)
        {
            return to_string(i);
        }
        else
        {
            cout << "[FileBuild.dll][FileTree::GetNum] The document is divided into too many parts" << endl;
            while (true)
            {
                // 打包终止
            }
        }
    }

    void WriteAsHash(node* n)
    {
        if (n->isFile)
        {
            // system("pause");
            n->data = File2Stream(n->name);
            cout << "[FileBuild.dll][FileTree::WriteAsHash] Read File " + n->name + " with " + to_string(n->data->size()) + " bytes" << endl;
            string path = n->name;
            vector<string> paths = vector<string>();
            string tmp = "";
            for (int i = 0; i < path.length(); i++)
            {
                if (path[i] == '\\')
                {
                    paths.push_back(tmp);
                    tmp = "";
                }
                else
                {
                    tmp += path[i];
                }
            }
            paths.push_back(tmp);
            string filename = ""; // 文件名
            string folderpath = ""; // 用于建立文件夹路径，这个应该是全是Hash的路径
            string filepath = ""; // 文件路径，这个也是带Hash的
            for (int i = 0; i < paths.size(); i++)
            {
                if (i != paths.size() - 1)
                {
                    folderpath.append(CPP_GetMD5(paths[i]) + "/");
                }
                else
                {
                    filename = CPP_GetMD5(paths[i]);
                }
                filepath.append(CPP_GetMD5(paths[i]) + "/");
            }
            filepath[filepath.length() - 1] = '.';
            std::filesystem::create_directories(folderpath); // 真正建立文件夹路径
            Stream2File(filepath, n-> data); // 真正建立文件

            // 生成客户端数字签名
            if (n->data->size() == 0)
            {
                pathinfo += n->name + " " + to_string(n->data->size()) + " empty";
            }
            else
            {
                string str = "";
                for (int i = 0; i < n->data->size(); i++)
                {
                    str += n->data->at(i);
                }
                pathinfo += n->name + " " + to_string(n->data->size()) + " " + CPP_GetMD5(str);
            }

            // 下面是压缩
            string pack30m = " -v3m -v3m -v3m -v3m -v3m -v3m -v3m -v3m -v3m -v3m";
            string pack300m = pack30m + pack30m + pack30m + pack30m + pack30m + pack30m + pack30m + pack30m + pack30m + pack30m;
            string pack3g = pack300m + pack300m + pack300m + pack300m + pack300m + pack300m + pack300m + pack300m + pack300m + pack300m;
            string cmd7z = string("7z.exe a download/") + CPP_GetMD5(path) + string(" ") + filepath.substr(0, filepath.length() - 1) + pack3g;
            int result = system(cmd7z.c_str());
            // std::filesystem::remove(filepath.substr(0, filepath.length() - 1));

            // 检查打包结果
            int packcount = 1;
            while (FileExist("download/" + CPP_GetMD5(path) + ".7z." + GetNum(packcount++)));
            packcount -= 2;
            pathinfo += " " + to_string(packcount) + "\n"; // 这是不规范的，应该写入\r\n
        }
        else
        {
            for (int i = 0; i < n->child.size(); i++)
            {
                WriteAsHash(n->child[i]);
            }
        }
    }

    void WriteInfoAsFile()
    {
        vector<char>* buffer = new vector<char>();
        for (int i = 0; i < pathinfo.length(); i++)
        {
            buffer->push_back(pathinfo[i]);
        }
        Stream2File("download/build.config", buffer);
        delete buffer;
    }
};

void CPP_BuildFile(string str)
{
    cout << "[FileBuild.dll::BuildFile] ReadFolder:" + str << endl;
    string* newName = new string(str);
    FileTree* ft = new FileTree(newName);
    ft->CmdShowNode(ft->root);
    system("pause");
    system("CLS");
    cout << "[FileBuild.dll::BuildFile] All file names overwrite as MD5" << endl;
    cout << "[FileBuild.dll::BuildFile] Aboout to read all file and rebuild as MD5 names" << endl;
    std::filesystem::create_directories("download"); // 建立下载文件夹
    ft->WriteAsHash(ft->root);
    cout << "Pack Finish! ";
    system("pause");
    system("CLS");
    cout << "[FileBuild.dll::BuildFile] Write build.config" << endl;
    ft->WriteInfoAsFile();
    cout << "[FileBuild.dll::BuildFile] =====All Finish!=====" << endl;
    system("pause");
    system("CLS");
}

bool CPP_CheckFile(string path, string md5)
{
    ifstream file(path, std::ios::binary | std::ios::in);
    if (!file)
    {
        return false;
    }
    streamsize fileStart = file.tellg();
    file.seekg(0, std::ios::end);
    streamsize fileSize = file.tellg() - fileStart;
    file.seekg(0, std::ios::beg);
    vector<char>* buffer = new vector<char>(fileSize);
    if (!file.read(buffer->data(), fileSize))
    {
        return false;
    }
    file.close();
    string str = "";
    for (int i = 0; i < buffer->size(); i++)
    {
        str += buffer->at(i);
    }
    if (md5 != CPP_GetMD5(str))
    {
        if (md5 == string("empty") || str.length() == 0)
        {
            delete buffer;
            return true;
        }
        std::filesystem::remove(path); // 校验错误的文件，不要了，重新下载！
        delete buffer;
        return false;
    }
    delete buffer;
    return true;
}

bool C_CheckFile(char* path, char* md5)
{
    return CPP_CheckFile(string(path), string(md5));
}

int* C_WriteAsHash(char* folder) // 文件夹下所有文件重命名为Hash，并兼顾了生成
{
    string str = string(folder);
    FileTree* ft = new FileTree(&str);
    std::filesystem::create_directories("download");
    ft->WriteAsHash(ft->root);
    return (int*)ft;
}

void C_WriteInfo(int* intptr)
{
    FileTree* ft = (FileTree*)intptr;
    ft->WriteInfoAsFile();
}