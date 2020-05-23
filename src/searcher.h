#pragma once
#ifndef INCLUDE_SRC_SEARCHER
#define INCLUDE_SRC_SEARCHER

#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include <cqcppsdk/cqcppsdk.h>
#include <nlohmann/json.hpp>

std::string arrayTostring(const std::vector<std::string> &str);

std::string getCardInfo(nlohmann::json targetInfo);

std::string getCardInfoEnglish(nlohmann::json targetInfo);

std::string searchCard(const std::string &msg, nlohmann::json &cardInfo, nlohmann::json &nicknameInfo,
                       nlohmann::json &eggInfo);

bool checkIfSearchCard(const std::string &msg);

std::string gethex(unsigned int c);

std::string encode(std::string str);

std::string searchBaidu(const std::string &msg);

bool checkIfSearchBaidu(const std::string &msg);

std::string searchInfo(const std::string &msg);

bool checkIfSearchInfo(const std::string &msg);

std::string getWelcomeMessage();

std::string getHelpMessage();

std::string getAskForFightMessage();

#endif //INCLUDE_SRC_SEARCHER
