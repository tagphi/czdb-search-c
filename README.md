# czdb C语言解析程序

查询 **czdb** IP库的C语言实现

## 前提条件

- CMake 3.10 或更高版本
- GCC编译器
- OpenSSL库

## 构建项目

1. 打开项目根目录的终端。

2. 创建一个新的构建目录并进入该目录：
    ```bash
    mkdir build
    cd build
    ```

3. 运行CMake以生成Makefile：
    ```bash
    cmake ..
    ```
   你也可以指定openssl库的路径：
    ```bash
    cmake OPENSSL_ROOT_DIR=/usr/local/opt/openssl;OPENSSL_LIBRARIES=/usr/local/opt/openssl/lib ..
    ```

4. 使用生成的Makefile编译项目：
    ```bash
    make
    ```

5. 运行编译后的程序：
    ```bash
    ./CZDBSearcher
    ```

## 许可

此项目根据Apache2许可证进行许可 - 有关详细信息，请参阅`LICENSE.md`文件。