## 游戏下载解决方案

`文档更新日期2024/9/4`

此方案用于普通Windows程序的网站下载

压缩方式：7z分卷压缩

校验方式：文件树md5校验，单文件内容md5校验

支持文件单个下载，用于校验资源和补充缺失文件

游戏更新：只需要替换服务器上的`download`即可（替换方案在末尾描述）

-------------------------------

## 使用本协议

### 包体要求

+ 路径必须是英文
+ 单个文件不应大于**2G**大小
+ 可以有空文件

### 不编译直接使用

##### 网站内容

直接运行Release中的程序即可，将会自动将路径下的`Game`打包为`download`文件夹

`download`中包含了`build.config`和`.7z.xxx`，分别是校验档案和游戏压缩的文件

直接把`download`提交至服务器即可

##### 客户端内容

先下载`download/build.config`

接着还原`Game`文件夹，**后面会提交一个客户端样例**

以一个配置单为例

```
Game\fonts\fzcq.ttf 2484512 2167a0f0bf3f1cb718f2683d13a4c887 3
Game\launcher.log 78 394b50aac88cee1cf063909945713928 1
Game\logo.ico 270398 6fb38ffb714d6d7d1e12697513fef822 1
Game\main.pak 88343377 b408488527227b5e0c017fe9b8378eae 24
```

分别是以空格分开的`[文件路径] [文件长度] [文件内容MD5] [文件分卷个数]`

以第一个文件为例 `Game\fonts\fzcq.ttf`

客户端应该下载的 `uri` 分别是

```
download/c232cf5a5f9bad7f14b78c0d894cedf1.7z.001
download/c232cf5a5f9bad7f14b78c0d894cedf1.7z.002
download/c232cf5a5f9bad7f14b78c0d894cedf1.7z.003
```

这里的 `c232cf5a5f9bad7f14b78c0d894cedf1` 是文件路径 `Game\fonts\fzcq.ttf` 的MD5

**生成文件时会保有一个中间文件夹，名字为一串MD5，必须删除 `download` 和这个中间文件夹才能重新打包，不然会报错**
