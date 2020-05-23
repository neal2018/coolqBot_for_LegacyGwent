#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include <cqcppsdk/cqcppsdk.h>
#include <nlohmann/json.hpp>

#include "data.h"
#include "searcher.h"

using json = nlohmann::json;
using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;

CQ_INIT {
    on_enable([] {
        ifstream f("/home/user/coolq/data/cardInfo.json");
        // ifstream f("/mnt/c/Users/neal/Projects/coolqBot_for_LegacyGwent/src/cardInfo.json");
        cardInfo = json::parse(f);
        logging::info("启用", "插件已启用");
    });

    on_private_message([](const PrivateMessageEvent &event) {
        if (checkIfSearchCard(event.message)) {
            try {
                const string msg = searchCard(event.message);
                send_message(event.target, msg); // 发送群消息
            } catch (ApiError &err) {
                logging::warning("群聊", "群聊消息失败, 错误码: " + to_string(err.code));
            }
        } else if (checkIfSearchBaidu(event.message)) {
            try {
                const string msg = searchBaidu(event.message);
                send_message(event.target, msg); // 发送群消息
            } catch (ApiError &err) {
                logging::warning("群聊", "群聊消息失败, 错误码: " + to_string(err.code));
            }
        } else if (checkIfSearchInfo(event.message)) {
            try {
                const string msg = searchInfo(event.message);
                send_message(event.target, msg); // 发送群消息
            } catch (ApiError &err) {
                logging::warning("群聊", "群聊消息失败, 错误码: " + to_string(err.code));
            }
        } else if (event.message.substr(0, 8) == "/welcome") {
            const string msg =
                "[WELCOME] 新人好！本群主要讨论的是老昆特相关事宜！\n群文件/"
                "客户端里面可以下载游戏，带有DIY的是DIY版本！\nzip结尾的是电脑版，apk结尾的是安卓版，dmg结尾的是mac"
                "版\n"
                "在客户端文件夹外也有DIY服的语音抢先体验版，欢迎下载！\nDIY服的更新内容在：https://shimo.im/"
                "docs/TQdjjwpPwd9hJhKc\n DIY的修改意见在：https://shimo.im/docs/hRIn0C91IFUYZZ6n\n期待你的参与！";
            send_message(event.target, msg); // 发送群消息
        } else {
            auto foundhc = event.message.find("hc");
            if (foundhc != string::npos) {
                try {
                    send_message(event.target, "hc爬！"); // 发送群消息
                } catch (ApiError &err) {
                    logging::warning("群聊", "群聊消息失败, 错误码: " + to_string(err.code));
                }
            }
        }
    });

    on_message([](const MessageEvent &event) {
        logging::debug("消息", "收到消息: " + event.message + "\n实际类型: " + typeid(event).name());
    });

    on_group_message([](const GroupMessageEvent &event) {
        if (checkIfSearchCard(event.message)) {
            try {
                const string msg = searchCard(event.message);
                send_group_message(event.group_id, msg); // 发送群消息
            } catch (ApiError &err) {
                logging::warning("群聊", "群聊消息失败, 错误码: " + to_string(err.code));
            }
        } else if (checkIfSearchBaidu(event.message)) {
            try {
                const string msg = searchBaidu(event.message);
                send_group_message(event.group_id, msg); // 发送群消息
            } catch (ApiError &err) {
                logging::warning("群聊", "群聊消息失败, 错误码: " + to_string(err.code));
            }
        } else if (checkIfSearchInfo(event.message)) {
            try {
                const string msg = searchInfo(event.message);
                send_group_message(event.group_id, msg); // 发送群消息
            } catch (ApiError &err) {
                logging::warning("群聊", "群聊消息失败, 错误码: " + to_string(err.code));
            }
        } else if (event.message.substr(0, 8) == "/welcome") {
            const string msg =
                "[WELCOME] 新人好！本群主要讨论的是老昆特相关事宜！\n群文件/"
                "客户端里面可以下载游戏，带有DIY的是DIY版本！\nzip结尾的是电脑版，apk结尾的是安卓版，dmg结尾的是mac"
                "版\n"
                "在客户端文件夹外也有DIY服的语音抢先体验版，欢迎下载！\nDIY服的更新内容在：https://shimo.im/"
                "docs/TQdjjwpPwd9hJhKc\n DIY的修改意见在：https://shimo.im/docs/hRIn0C91IFUYZZ6n\n期待你的参与！";
            send_group_message(event.group_id, msg); // 发送群消息
        } else if (event.message == "/help" || event.message == "/h") {
            const string msg =
                "[HELP] 发送以下命令可以触发相应效果：\n"
                "/help 或 /h：显示本信息\n"
                "/info：显示相关的信息\n"
                "/welcome：发送入群欢迎信息\n"
                "/searchcard 或 /sc + 关键词：搜索卡牌效果\n"
                "/baidu 或 /bd + 关键词：使用百度搜索\n"
                "小助手期待大家多多打牌！";
            send_group_message(event.group_id, msg);
        } else {
            auto foundhc = event.message.find("hc");
            if (foundhc != string::npos) {
                try {
                    send_group_message(event.group_id, "hc爬！"); // 发送群消息
                } catch (ApiError &err) {
                    logging::warning("群聊", "群聊消息失败, 错误码: " + to_string(err.code));
                }
            } else {
                srand(time(NULL));
                if ((rand() % 200) == 0) {
                    try {
                        send_group_message(event.group_id, "有没有人来打一把，嘤嘤嘤"); // 发送群消息
                    } catch (ApiError &err) {
                        logging::warning("群聊", "群聊消息失败, 错误码: " + to_string(err.code));
                    }
                }
            }
        }
    });

    on_group_member_increase([](const GroupMemberIncreaseEvent &event) {
        try {
            const string msg =
                "[WELCOME] 新人好！本群主要讨论的是老昆特相关事宜！\n群文件/"
                "客户端里面可以下载游戏，带有DIY的是DIY版本！\nzip结尾的是电脑版，apk结尾的是安卓版，dmg结尾的是mac"
                "版\n"
                "在客户端文件夹外也有DIY服的语音抢先体验版，欢迎下载！\nDIY服的更新内容在：https://shimo.im/"
                "docs/TQdjjwpPwd9hJhKc\n DIY的修改意见在：https://shimo.im/docs/hRIn0C91IFUYZZ6n\n期待你的参与！";
            send_group_message(event.group_id, msg); // 发送群消息
        } catch (ApiError &err) {
            logging::warning("群聊", "群聊消息失败, 错误码: " + to_string(err.code));
        }
    });
}
