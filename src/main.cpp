#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include <cqcppsdk/cqcppsdk.h>
#include <nlohmann/json.hpp>

#include "searcher.h"

using json = nlohmann::json;
using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;

const string DATA_PATH = "/home/user/coolq/data/cardData/";
// const string DATA_PATH = "/mnt/c/Users/neal/Projects/coolqBot_for_LegacyGwent/src/cardData/";

bool hcSwitch = false;

const int32_t GROUP_ID = 945408322;

json cardInfo;
json nicknameInfo;
json eggInfo;

CQ_INIT {
    on_enable([] {
        ifstream cardInfoFile(DATA_PATH + "cardInfo.json");
        cardInfo = json::parse(cardInfoFile);
        ifstream nicknameInfoFile(DATA_PATH + "nicknameInfo.json");
        nicknameInfo = json::parse(nicknameInfoFile);
        ifstream eggInfoFile(DATA_PATH + "eggInfo.json");
        eggInfo = json::parse(eggInfoFile);
        logging::info("启用", "插件已启用");
    });

    on_private_message([](const PrivateMessageEvent &event) {
        if (checkIfSearchCard(event.message)) {
            try {
                const string msg = searchCard(event.message, cardInfo, nicknameInfo, eggInfo);
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
            const string msg = getWelcomeMessage();
            send_message(event.target, msg); // 发送群消息
        } else if (event.message.substr(0, 5) == "/send") {
            auto found = event.message.find("来一把");
            if (found != string::npos) {
                const string msg = getAskForFightMessage();
                send_group_message(GROUP_ID, msg);
            }
        } else if (event.message.substr(0, 6) == "/offhc") {
            hcSwitch = false;
        } else if (event.message.substr(0, 5) == "/onhc") {
            hcSwitch = true;
        } else {
            auto foundhc = event.message.find("hc");
            if (foundhc != string::npos && hcSwitch) {
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
                const string msg = searchCard(event.message, cardInfo, nicknameInfo, eggInfo);
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
            const string msg = getWelcomeMessage();
            send_group_message(event.group_id, msg); // 发送群消息
        } else if (event.message == "/help" || event.message == "/h") {
            const string msg = getHelpMessage();
            send_group_message(event.group_id, msg);
        } else {
            auto foundhc = event.message.find("hc");
            if (foundhc != string::npos && hcSwitch) {
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
            const string msg = getWelcomeMessage();
            send_group_message(event.group_id, msg); // 发送群消息
        } catch (ApiError &err) {
            logging::warning("群聊", "群聊消息失败, 错误码: " + to_string(err.code));
        }
    });
}
