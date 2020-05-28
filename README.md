# CoolQ C++ SDK 项目模板

[![Build Status](https://img.shields.io/appveyor/ci/richardchien/cqcppsdk-template.svg)](https://ci.appveyor.com/project/richardchien/cqcppsdk-template)

本仓库是 [CoolQ C++ SDK](https://github.com/cqmoe/cqcppsdk) 的项目模板，提供了基本的目录结构、CMake 配置、持续集成配置、安装脚本等，使用方法请参考 [CoolQ C++ SDK 文档](https://cqcppsdk.cqp.moe/)。

## 自己增加的工作内容：
## 使用LIBUV 为了增加SOCKET库
1. 自己编译的LIBUV 
* 需要增加
```
libuv.lib
ws2_32.lib
IPHLPAPI.lib
Psapi.lib
userenv.lib
```
* 不能把uvcpp里的头文件放到解决方案里

2. 增加自己的sqlite3
注意SQLITE3的C文件，必须独立设置为C编译。
3. 增加JSONCPP库
