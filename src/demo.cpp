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

json eggInfo = {
    {"支柱", "支柱万岁！赞美支柱！"},
    {"尼哥", "尼哥爬"},
    {"hc", "hc爬"},
};

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

    if (eggInfo.contains(searchContent)) {
        return eggInfo[searchContent];
    }

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
    } else if (possibleAnswer.size() > 10) {
        string returnInfo = "[WARN] 搜索结果过多，请使用更精确的关键词。";
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

string gethex(unsigned int c) //参数必须是int，你懂的
{
    std::ostringstream stm;
    stm << '%' << std::hex << std::nouppercase << c;
    return stm.str();
}

string encode(string str) {
    static const std::string unreserved =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "-_.~";

    string r;

    for (unsigned int i = 0; i < str.length(); i += 1) {
        unsigned char c = str.at(i); //这里必须是无符号型，你懂得
        if (unreserved.find(c) != -1)
            r += c;
        else
            r += gethex(c);
    }

    return r;
}

string searchBaidu(const string &msg) {
    string searchContent = "";

    // remove prefix
    if (msg.substr(0, 6) == "/baidu") {
        searchContent = msg.substr(6);
    } else if (msg.substr(0, 3) == "/bd") {
        searchContent = msg.substr(3);
    } else {
        logging::warning("错误", "非法查询");
        return "";
    }

    // earse spaces
    auto itor = remove_if(searchContent.begin(), searchContent.end(), ::isspace);
    searchContent.erase(itor, searchContent.end());

    string hex = encode(searchContent);
    string searchUrl = "http://www.baidu.com/s?wd=" + hex;

    return "[INFO] 点击 " + searchUrl + " 查看搜索结果！";
}

bool checkIfSearchBaidu(const string &msg) {
    if (msg.substr(0, 6) == "/baidu" || msg.substr(0, 3) == "/bd") {
        return true;
    } else {
        return false;
    }
}

string searchInfo(const string &msg) {
    string searchContent = "";

    // remove prefix
    if (msg.substr(0, 5) == "/info") {
        searchContent = msg.substr(5);
    } else {
        logging::warning("错误", "非法查询");
        return "";
    }

    // earse spaces
    auto itor = remove_if(searchContent.begin(), searchContent.end(), ::isspace);
    searchContent.erase(itor, searchContent.end());

    auto foundDoc = searchContent.find("文档");
    if (foundDoc != string::npos) {
        return "[INFO] DIY服务器的更新内容在：https://shimo.im/docs/TQdjjwpPwd9hJhKc\n "
               "DIY的修改意见在：https://shimo.im/docs/hRIn0C91IFUYZZ6n\n欢迎大家踊跃参与！";
    }

    auto foundWeb = searchContent.find("在线");
    if (foundWeb != string::npos) {
        return "[INFO] DIY服的在线人数在：http://cynthia.ovyno.com:5005/\n "
               "原服的在线人数在：http://cynthia.ovyno.com:5000/\n欢迎大家一起打牌！";
    }

    auto foundWelcome = searchContent.find("下载");
    if (foundWelcome != string::npos) {
        return "[INFO] "
               "群文件/"
               "客户端里面可以下载游戏，带有DIY的是DIY版本！\nzip结尾的是电脑版，apk结尾的是安卓版，dmg结尾的是mac版\n"
               "在客户端文件夹外也有DIY服的语音抢先体验版，欢迎大家下载！\nDIY服务器的更新内容在：https://shimo.im/"
               "docs/TQdjjwpPwd9hJhKc\n DIY的修改意见在：https://shimo.im/docs/hRIn0C91IFUYZZ6n";
    }

    return "[INFO] /info后面可以输入【文档】、【在线】或者【下载】查看相关内容！";
}

bool checkIfSearchInfo(const string &msg) {
    if (msg.substr(0, 5) == "/info") {
        return true;
    } else {
        return false;
    }
}

CQ_INIT {
    on_enable([] {
        ifstream f("/mnt/c/Users/neal/Projects/coolqBot/src/cardInfo.json");
        cardInfo = json::parse(f);
        // std::cout << cardInfo;
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
        }else {
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
        } else {
            auto foundhc = event.message.find("hc");
            if (foundhc != string::npos) {
                try {
                    send_group_message(event.group_id, "hc爬！"); // 发送群消息
                } catch (ApiError &err) {
                    logging::warning("群聊", "群聊消息失败, 错误码: " + to_string(err.code));
                }
            }
        }
    });

    on_group_member_increase([](const GroupMemberIncreaseEvent &event) {
        try {
            const string msg =
                "[WELCOME] 新人好！本群主要讨论的是老昆特相关事宜！\n群文件/"
                "客户端里面可以下载游戏，带有DIY的是DIY版本！\nzip结尾的是电脑版，apk结尾的是安卓版，dmg结尾的是mac版\n"
                "在客户端文件夹外也有DIY服的语音抢先体验版，欢迎下载！\nDIY服的更新内容在：https://shimo.im/"
                "docs/TQdjjwpPwd9hJhKc\n DIY的修改意见在：https://shimo.im/docs/hRIn0C91IFUYZZ6n\n期待你的参与！";
            send_group_message(event.group_id, msg); // 发送群消息
        } catch (ApiError &err) {
            logging::warning("群聊", "群聊消息失败, 错误码: " + to_string(err.code));
        }
    });
}