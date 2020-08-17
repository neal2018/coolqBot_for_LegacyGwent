由于coolq停止服务，本项目已停止，迁移至https://github.com/neal2018/mirai_bot

---
# coolqBot for LegacyGwent
本项目为老昆特的qq机器人。

目前功能有：

发送
- /searchcard + 卡牌名称：可以查询卡牌。
- /info：可以查询老昆特相关信息
- /baidu + 搜索内容：可以查询百度
- /welcome：发送新人须知
- 私聊发送/send + 来一把：可以约战

# 部署方式
安装coolq后，clone本项目，然后下载子模块
```
git submodule update --init --recursive
```
然后把`src/cardData`文件夹存放到电脑中的任一位置，再把`main.cpp`中的文件地址
```
const string DATA_PATH = "/home/user/coolq/data/cardData/";
```
改为`src/cardData`文件夹存放的位置，然后按照coolq模板的[文档](https://cqcppsdk.cqp.moe/)执行打包即可。推荐直接使用[AppVeyor](https://ci.appveyor.com/)持续集成，使用方法也可参照coolq模板的[文档](https://cqcppsdk.cqp.moe/)。


# 项目模板
使用的模板来自：
CoolQ C++ SDK 项目模板

[![Build Status](https://img.shields.io/appveyor/ci/richardchien/cqcppsdk-template.svg)](https://ci.appveyor.com/project/richardchien/cqcppsdk-template)

该仓库是 [CoolQ C++ SDK](https://github.com/cqmoe/cqcppsdk) 的项目模板，提供了基本的目录结构、CMake 配置、持续集成配置、安装脚本等，使用方法请参考 [CoolQ C++ SDK 文档](https://cqcppsdk.cqp.moe/)。
