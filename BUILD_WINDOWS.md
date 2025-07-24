# Windows 编译指南

本项目现在支持在Windows平台下编译并生成DLL。以下是在Windows下编译的步骤：

## 依赖要求

### 1. Visual Studio 或 Visual Studio Build Tools
- 安装 Visual Studio 2017 或更新版本
- 或者安装 Visual Studio Build Tools

### 2. CMake
- 下载并安装 CMake 3.10 或更新版本
- 确保 CMake 在 PATH 环境变量中

### 3. vcpkg (推荐的包管理器)
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
```

### 4. 安装依赖库
使用 vcpkg 安装 OpenSSL 和 msgpack-c：
```bash
.\vcpkg install openssl:x64-windows
.\vcpkg install msgpack-c:x64-windows
```

## 编译步骤

### 使用 vcpkg
```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_GENERATOR_PLATFORM=x64
cmake --build . --config Release
```

### 手动指定依赖路径
如果不使用 vcpkg，可以手动指定依赖库的路径：
```bash
mkdir build
cd build
cmake .. -DMSGPACK_ROOT=C:/path/to/msgpack -DOPENSSL_ROOT_DIR=C:/path/to/openssl -DCMAKE_GENERATOR_PLATFORM=x64
cmake --build . --config Release
```

## 编译结果

编译成功后，你将得到：
- `db_searcher.dll` - 动态链接库
- `CZDBSearcher.exe` - 可执行文件
- `db_searcher.lib` - 导入库（用于链接DLL）

## 使用 DLL

在你的项目中使用 DLL：
1. 包含头文件 `db_searcher.h`
2. 链接 `db_searcher.lib`
3. 确保运行时能找到 `db_searcher.dll`

示例代码：
```c
#include "db_searcher.h"

int main() {
    DBSearcher* searcher = initDBSearcher("database.czdb", "your_key", MEMORY);
    if (searcher) {
        char region[100];
        if (search("8.8.8.8", searcher, region, 100) == 0) {
            printf("Region: %s\n", region);
        }
        closeDBSearcher(searcher);
    }
    return 0;
}
```

## 故障排除

### 找不到 OpenSSL
如果遇到 OpenSSL 找不到的错误，可以：
1. 使用 vcpkg 安装：`vcpkg install openssl:x64-windows`
2. 或者设置环境变量 `OPENSSL_ROOT_DIR` 指向 OpenSSL 安装目录

### 找不到 msgpack-c
如果遇到 msgpack-c 找不到的错误，可以：
1. 使用 vcpkg 安装：`vcpkg install msgpack-c:x64-windows`
2. 或者设置环境变量 `MSGPACK_ROOT` 指向 msgpack 安装目录

### DLL 依赖问题
运行时如果遇到DLL找不到的错误：
1. 确保所有依赖的DLL（OpenSSL、msgpack等）在PATH中
2. 或者将依赖的DLL复制到程序目录
3. 使用 `dumpbin /dependents your_program.exe` 查看依赖关系 