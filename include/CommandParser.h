#ifndef MONITOR_COMMAND_PARSER_H
#define MONITOR_COMMAND_PARSER_H

#include <string>

enum CommandType
{
    CMD_EMPTY,     // ligne vide, blanche ou commentaire
    CMD_SENSOR,
    CMD_READ,
    CMD_ACK,
    CMD_REPORT,
    CMD_INVALID    // voir Command::error
};

/// Resultat de l'analyse d'une ligne. Seuls les champs utiles au type
/// de commande sont renseignes ; les autres gardent leur valeur par defaut.
struct Command
{
    CommandType type;
    std::string sensorType;  // SENSOR
    std::string sensorId;    // SENSOR, READ, ACK
    std::string label;       // SENSOR
    int minValue;            // SENSOR
    int maxValue;            // SENSOR
    int time;                // READ, ACK, REPORT
    int value;               // READ
    std::string error;       // CMD_INVALID uniquement

    Command();
};

/// Analyse syntaxique d'une ligne du fichier de scenario.
/// Ne connait ni les capteurs ni les alarmes : verifie seulement la forme.
class CommandParser
{
public:
    static Command parse(const std::string& line);

private:   // --- partie privée : libre, tu peux la modifier ---
    CommandParser();  // classe purement statique, non instanciable
};

#endif
