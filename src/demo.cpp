#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

#include <cqcppsdk/cqcppsdk.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;

const string CardMapVersion = "1.0.0.9";
json cardInfo;

string searchCard(const string &msg) {
    string searchContent = "";

    // remove prefix
    if (msg.substr(0, 11) == "/searchcard") {
        searchContent = msg.substr(11);
    } else if (msg.substr(0, 6) == "/scard") {
        searchContent = msg.substr(6);
    } else if (msg.substr(0, 3) == "/sc") {
        searchContent = msg.substr(3);
    } else {
        logging::warning("错误", "非法查询");
        return "";
    }

    // earse spaces
    auto itor = remove_if(searchContent.begin(), searchContent.end(), ::isspace);
    searchContent.erase(itor, searchContent.end());

    if (cardInfo.contains(searchContent)) {
        json targetInfo = cardInfo[searchContent];

        // change the list into string
        vector<string> categoriesArray = targetInfo["Categories"];
        string categoriesString = "";
        for (const string &s : categoriesArray) {
            categoriesString += s;
        }

        string returnInfo =
            "CardId: " + targetInfo["CardId"].get<string>() + '\n' + "Name: " + targetInfo["Name"].get<string>() + '\n'
            + "Strength: " + to_string(targetInfo["Strength"].get<int>()) + '\n'
            + "Info: " + targetInfo["Info"].get<string>() + '\n' + "Group: " + targetInfo["Group"].get<string>() + '\n'
            + "Faction: " + targetInfo["Faction"].get<string>() + '\n' + "Categories: " + categoriesString + '\n';
        return returnInfo;
    }

    vector<string> possibleAnswer;

    for (auto it = cardInfo.begin(); it != cardInfo.end(); ++it) {
        json nestedInfo = *it;
        string name = nestedInfo["Name"].get<string>();
        string info = nestedInfo["Info"].get<string>();

        std::size_t foundName = name.find(searchContent);
        std::size_t foundInfo = info.find(searchContent);
        if (foundName != string::npos || foundInfo != string::npos) {
            possibleAnswer.push_back(nestedInfo["CardId"]);
        }
    }

    if (possibleAnswer.size() == 0) {
        return "[WARN] 没有找到符合条件的个体。";
    } else if (possibleAnswer.size() == 1) {
        json targetInfo = cardInfo[possibleAnswer[0]];

        // change the list into string
        vector<string> categoriesArray = targetInfo["Categories"];
        string categoriesString = "";
        for (const string &s : categoriesArray) {
            categoriesString += s;
        }

        string returnInfo =
            "CardId: " + targetInfo["CardId"].get<string>() + '\n' + "Name: " + targetInfo["Name"].get<string>() + '\n'
            + "Strength: " + to_string(targetInfo["Strength"].get<int>()) + '\n'
            + "Info: " + targetInfo["Info"].get<string>() + '\n' + "Group: " + targetInfo["Group"].get<string>() + '\n'
            + "Faction: " + targetInfo["Faction"].get<string>() + '\n' + "Categories: " + categoriesString + '\n';
        return returnInfo;
    } else {
        string returnInfo = "符合条件的卡牌有：";
        for (const string &s : possibleAnswer) {
            returnInfo +=
                ' ' + cardInfo[s]["Name"].get<string>() + '(' + cardInfo[s]["CardId"].get<string>() + ')' + ",";
        }
        returnInfo.pop_back();
        return returnInfo;
    }
}

bool checkIfSearchCard(const string &msg) {
    if (msg.substr(0, 11) == "/searchcard" || msg.substr(0, 6) == "/scard" || msg.substr(0, 3) == "/sc") {
        return true;
    } else {
        return false;
    }
}

CQ_INIT {
    on_enable([] {
        ifstream f("/home/user/coolq/data/cardInfo.json");
        cardInfo = json::parse(f);
        // std::cout << cardInfo;
        logging::info("启用", "插件已启用");
    });

    on_private_message([](const PrivateMessageEvent &event) {
        if (checkIfSearchCard(event.message)) {
            try {
                const string msg = searchCard(event.message);
                send_message(event.target, msg); // 使用 message 模块构造消息
            } catch (ApiError &err) {
                logging::warning("私聊", "私聊消息失败, 错误码: " + to_string(err.code));
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
        }
    });
}