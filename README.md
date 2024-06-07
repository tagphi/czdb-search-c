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
   
   你需要openssl库编译程序，如果你是mac环境并且没有安装过可以通过下面方式安装
    ```bash
    brew install openssl
    ```
   安装完后设置环境变量OPENSSL_ROOT_DIR和OPENSSL_LIBRARIES，然后运行cmake

    ```bash
    cmake ..
    ```
   你也可以指定openssl库的路径：
    ```bash
    cmake -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl -DOPENSSL_LIBRARIES=/usr/local/opt/openssl/lib ..
    ```

4. 使用生成的Makefile编译项目：
    ```bash
    make
    ```
   
   这将创建一个libdb_searcher.dylib库和一个CZDBSearcher可执行文件。

5. 运行编译后的程序：
    ```bash
    ./CZDBSearcher -p /path/to/ipdbfile -k key
    ```
## 使用

参考main.c文件。

在不需要查询的时候调用closeDBSearcher(dbSearcher)销毁DBSearcher。注意不要每次查询都去创建一个DBSearcher，这样会导致文件句柄泄漏，也会造成不必要的资源消耗。

一个可行的方法是为每个线程创建一个DBSearcher，然后在线程销毁的时候销毁DBSearcher。

## 线程安全
支持2种查询方式Memory和Btree，Memory方式是线程安全的，Btree方式不是线程安全的。这主要是因为DBSearcher这个结构种保存了文件句柄，如果多个线程同时访问会导致文件指针错乱。

## 许可

此项目根据Apache2许可证进行许可 - 有关详细信息，请参阅`LICENSE`文件。