#ifndef BACKEND_H
#define BACKEND_H
#include <nlohmann/json.hpp>
#include <iostream>
#include "connection.h"
#include "instruction_manager.h"
#include "variant_const.h"
#include "logger.h"

using json=nlohmann::json;


bool CheckIfQuery(nlohmann::basic_json<>& rqs);

/* bool CheckIfQueryRes(nlohmann::basic_json<>& rqs); */
bool CheckIfSetting(nlohmann::basic_json<>& rqs);

// check json if valid
bool CheckIfValid(nlohmann::basic_json<>& rqs);

// return instruction
std::string GetInstruction(nlohmann::basic_json<>& rqs);

#include <bitset>
// return json str
std::string GetResponse(std::shared_ptr<InsParser> parser, std::string ipv4);

std::string ProcessRequest(std::string data_str);

#endif
