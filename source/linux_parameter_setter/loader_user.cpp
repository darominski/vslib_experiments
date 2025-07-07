#include <algorithm>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "json/json.hpp"
#include "shared_memory.h"

using namespace vslib;
using namespace fgcd;
using namespace fgc4;

std::vector<double> processDoubleArray(const std::string& value)
{
    std::vector<double> array;

    std::stringstream ss(value);
    std::string       item;

    while (getline(ss, item, ' '))
    {
        try
        {
            array.push_back(stod(item));
        }
        catch (std::bad_cast& e)
        {
            std::cerr << e.what() << std::endl;
            continue;
        }
    }
    return array;
}

std::vector<int> processIntArray(const std::string& value)
{
    std::vector<int>  array;
    std::stringstream ss(value);
    std::string       item;

    while (getline(ss, item, ' '))
    {
        try
        {
            array.push_back(stoi(item));
        }
        catch (std::bad_cast& e)
        {
            std::cerr << e.what() << std::endl;
            continue;
        }
    }
    return array;
}

nlohmann::json prepareCommand(const std::string& name, const std::string& type, const std::string& value)
{
    nlohmann::json command = nlohmann::json::object();
    command["name"]        = name;

    std::string lowercase_type;
    std::transform(
        type.begin(), type.end(), std::back_inserter(lowercase_type),
        [](unsigned char c)
        {
            return std::tolower(c);
        }
    );

    if (lowercase_type.find("array") != std::string::npos)
    {
        if (lowercase_type.find("int") != std::string::npos)
        {
            command["value"] = processIntArray(value);
        }
        else if (lowercase_type.find("double") != std::string::npos)
        {
            command["value"] = processDoubleArray(value);
        }
    }
    else if (lowercase_type.find("int") != std::string::npos)
    {
        command["value"] = stoi(value);
    }
    else if (lowercase_type.find("float") != std::string::npos || lowercase_type.find("double") != std::string::npos)
    {
        command["value"] = stod(value);
    }

    else   // enum, string
    {
        command["value"] = value;
    }
    return command;
}

int main()
{
    Fgc4Shmem shared_memory;

    auto const json_manifest = readJsonFromSharedMemory(&SHARED_MEMORY);
    std::cout << json_manifest.dump(2) << "\n";

    std::cout << std::endl;
    std::cout << "Please input the command in the following format: name type value" << std::endl;
    while (true)
    {
        if (SHARED_MEMORY.acknowledged_counter < SHARED_MEMORY.transmitted_counter)
        {
            // first process not ready to receive more commands, wait and skip to next iteration
            usleep(500000);   // 0.5 s
            continue;
        }
        std::string name  = "";
        std::string type  = "";
        std::string value = "";
        std::string command_string;
        std::getline(std::cin, command_string);

        name           = command_string.substr(0, command_string.find_first_of(' '));
        command_string = command_string.substr(command_string.find_first_of(' ') + 1);
        type           = command_string.substr(0, command_string.find_first_of(' '));
        value          = command_string.substr(command_string.find_first_of(' ') + 1);

        const auto command = prepareCommand(name, type, value);
        std::cout << "Command sent: " << command.dump(2) << std::endl;

        writeJsonToSharedMemory(command, &SHARED_MEMORY);
        SHARED_MEMORY.transmitted_counter++;
    }

    return 0;
}