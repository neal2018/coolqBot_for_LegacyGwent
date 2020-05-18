# coolqBot for LegacyGwent
本项目为老昆特的qq机器人。

目前功能有：

发送
- /searchcard + 卡牌名称：可以查询卡牌。
- /info：可以查询老昆特相关信息
- /baidu + 搜索内容：可以查询百度
- /welcome：发送新人须知

# 部署方式
安装coolq后，把`cardInfo.json`存放到电脑中的任一位置，然后把`demo.cpp`中的文件地址
```
ifstream f("/home/user/coolq/data/cardInfo.json");
```
改为`cardInfo.json`存放的位置即可。


#项目模板
使用的模板来自：
CoolQ C++ SDK 项目模板

[![Build Status](https://img.shields.io/appveyor/ci/richardchien/cqcppsdk-template.svg)](https://ci.appveyor.com/project/richardchien/cqcppsdk-template)

本仓库是 [CoolQ C++ SDK](https://github.com/cqmoe/cqcppsdk) 的项目模板，提供了基本的目录结构、CMake 配置、持续集成配置、安装脚本等，使用方法请参考 [CoolQ C++ SDK 文档](https://cqcppsdk.cqp.moe/)。
