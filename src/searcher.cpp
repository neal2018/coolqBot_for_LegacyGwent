#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include <cqcppsdk/cqcppsdk.h>
#include <nlohmann/json.hpp>

#include "data.h"

using json = nlohmann::json;
using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;

string arrayToString(const vector<string> &str) {
    string retrunString = "";
    for (const string &s : str) {
        retrunString += s + " ";
    }
    return retrunString;
}

string getCardInfo(json targetInfo) {
    vector<string> categoriesArray = targetInfo["Categories"];
    string categoriesString = arrayToString(categoriesArray);

    return "卡牌Id: " + targetInfo["CardId"].get<string>() + '\n' + "名称: " + targetInfo["Name"].get<string>() + '\n'
           + "点数: " + to_string(targetInfo["Strength"].get<int>()) + '\n'
           + "效果: " + targetInfo["Info"].get<string>() + '\n' + "品质: " + targetInfo["Group"].get<string>() + '\n'
           + "势力: " + targetInfo["Faction"].get<string>() + '\n' + "标签: " + categoriesString + '\n'
           + "背景故事: " + targetInfo["Flavor"].get<string>();
}

string getCardInfoEnglish(json targetInfo) {
    vector<string> categoriesArray = targetInfo["Categories"];
    string categoriesString = arrayToString(categoriesArray);
    for (const string &s : categoriesArray) {
        categoriesString += s + " ";
    }
    return "CardId: " + targetInfo["CardId"].get<string>() + '\n' + "Name: " + targetInfo["Name"].get<string>() + '\n'
           + "Strength: " + to_string(targetInfo["Strength"].get<int>()) + '\n'
           + "Info: " + targetInfo["Info"].get<string>() + '\n' + "Group: " + targetInfo["Group"].get<string>() + '\n'
           + "Faction: " + targetInfo["Faction"].get<string>() + '\n' + "Categories: " + categoriesString + '\n'
           + "Story: " + targetInfo["Flavor"].get<string>();
}

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
        string returnInfo = getCardInfo(targetInfo);
        return returnInfo;
    }

    vector<string> possibleAnswer;
    unordered_set<string> possibleAnswerSet;
    for (auto it = cardInfo.begin(); it != cardInfo.end(); ++it) {
        json nestedInfo = *it;

        string name = nestedInfo["Name"].get<string>();
        string info = nestedInfo["Info"].get<string>();
        string flavor = nestedInfo["Flavor"].get<string>();

        size_t foundName = name.find(searchContent);
        size_t foundInfo = info.find(searchContent);
        size_t foundFlavor = flavor.find(searchContent);
        if (foundName != string::npos || foundInfo != string::npos || foundFlavor != string::npos) {
            string cardId = nestedInfo["CardId"].get<string>();
            possibleAnswer.push_back(cardId);
            possibleAnswerSet.insert(cardId);
        }
    }

    for (auto &it : nicknameInfo.items()) {
        string nickname = it.value();
        string cardId = it.key();
        if (possibleAnswerSet.count(cardId) != 0) continue;
        size_t foundNickname = nickname.find(searchContent);
        if (foundNickname != string::npos) {
            possibleAnswer.push_back(cardId);
        }
    }

    if (possibleAnswer.size() == 0) {
        return "[WARN] 没有找到符合条件的个体。要不要试试/baidu呢？";
    } else if (possibleAnswer.size() == 1) {
        json targetInfo = cardInfo[possibleAnswer[0]];

        string returnInfo = getCardInfo(targetInfo);
        return returnInfo;
    } else if (possibleAnswer.size() > 12) {
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

string gethex(unsigned int c) {
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
        unsigned char c = str.at(i);
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

    auto foundDownload = searchContent.find("下载");
    if (foundDownload != string::npos) {
        return "[INFO] "
               "群文件/"
               "客户端里面可以下载游戏，带有DIY的是DIY版本！\nzip结尾的是电脑版，apk结尾的是安卓版，dmg结尾的是mac"
               "版\n"
               "在客户端文件夹外也有DIY服的语音抢先体验版，欢迎大家下载！\nDIY服务器的更新内容在：https://shimo.im/"
               "docs/TQdjjwpPwd9hJhKc\n DIY的修改意见在：https://shimo.im/docs/hRIn0C91IFUYZZ6n";
    }

    auto foundPairCode = searchContent.find("匹配码");
    if (foundPairCode != string::npos) {
        return "[INFO] "
               "匹配界面右下角可以输入匹配码，匹配码一样的玩家可以相互好友对战！\n"
               "匹配码输入ai或者ai1可以匹配ai，但会优先匹配在线玩家。使用ai#f或者ai1#f可以强制匹配ai。";
    }

    return "[INFO] /info后面可以输入【文档】、【在线】、【匹配码】或者【下载】查看相关内容！";
}

bool checkIfSearchInfo(const string &msg) {
    if (msg.substr(0, 5) == "/info") {
        return true;
    } else {
        return false;
    }
}