#include "CommandParser.h"
#include <sstream>
#include <vector>

Command::Command()
    :type(CMD_EMPTY),
    sensorType(""),
    sensorId(""),
    label(""),
    minValue(0),
    maxValue(0),
    time(0),
    value(0),
    error("")
{
}

// Renvoie true si 'word' est entierement un entier, et le met dans 'out'.
static bool toInt(const std::string& word, int& out)
{
    std::istringstream iss(word);
    char extra;
    if (!(iss >> out)){
        return false;          // "abc" : la lecture echoue
    }
    if (iss >> extra){
        return false;          // "12abc", "1.5" : il reste des caracteres
    }
    return true;
}

// Lit un TIME : entier valide et >= 0.
static bool toTime(const std::string& word, int& out)
{
    return toInt(word, out) && out >= 0;
}

static Command invalid(const std::string& message)
{
    Command cmd;
    cmd.type = CMD_INVALID;
    cmd.error = message;
    return cmd;
}

Command CommandParser::parse(const std::string& line){
    Command cmd;
    std::istringstream iss(line);
    std::vector<std::string> words;
    std::string w;
    while (iss >> w){
        words.push_back(w);
    }

    // Ligne vide, blanche ou commentaire
    if (words.empty() || words[0][0] == '#'){
        return cmd;
    }

    const std::string& keyword = words[0];
    const std::string malformed = "malformed " + keyword + " command";

    if (keyword == "SENSOR"){
        // SENSOR <TYPE> <ID> <LABEL> <MIN> <MAX>
        if (words.size() != 6
            || !toInt(words[4], cmd.minValue)
            || !toInt(words[5], cmd.maxValue)){
            return invalid(malformed);
        }
        cmd.type = CMD_SENSOR;
        cmd.sensorType = words[1];
        cmd.sensorId = words[2];
        cmd.label = words[3];
    }
    else if (keyword == "READ"){
        // READ <TIME> <ID> <VALUE>
        if (words.size() != 4
            || !toTime(words[1], cmd.time)
            || !toInt(words[3], cmd.value)){
            return invalid(malformed);
        }
        cmd.type = CMD_READ;
        cmd.sensorId = words[2];
    }
    else if (keyword == "ACK"){
        // ACK <TIME> <ID>
        if (words.size() != 3 || !toTime(words[1], cmd.time)){
            return invalid(malformed);
        }
        cmd.type = CMD_ACK;
        cmd.sensorId = words[2];
    }
    else if (keyword == "REPORT"){
        // REPORT <TIME>
        if (words.size() != 2 || !toTime(words[1], cmd.time)){
            return invalid(malformed);
        }
        cmd.type = CMD_REPORT;
    }
    else {
        return invalid("unknown command '" + keyword + "'");
    }

    return cmd;
}