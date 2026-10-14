#include <string>

#include "CommandParser.h"
#include "check.h"

static void testEmptyAndComments()
{
    CHECK_EQ(CommandParser::parse("").type, CMD_EMPTY);
    CHECK_EQ(CommandParser::parse("   \t  ").type, CMD_EMPTY);
    CHECK_EQ(CommandParser::parse("# commentaire").type, CMD_EMPTY);
    CHECK_EQ(CommandParser::parse("   #indente").type, CMD_EMPTY);
}

static void testSensor()
{
    Command c = CommandParser::parse("SENSOR TEMP T1 ENG1_EGT -50 900");
    CHECK_EQ(c.type, CMD_SENSOR);
    CHECK_EQ(c.sensorType, std::string("TEMP"));
    CHECK_EQ(c.sensorId, std::string("T1"));
    CHECK_EQ(c.label, std::string("ENG1_EGT"));
    CHECK_EQ(c.minValue, -50);
    CHECK_EQ(c.maxValue, 900);
}

static void testExtraSpaces()
{
    Command c = CommandParser::parse("  READ   12\tP1    -3  ");
    CHECK_EQ(c.type, CMD_READ);
    CHECK_EQ(c.time, 12);
    CHECK_EQ(c.sensorId, std::string("P1"));
    CHECK_EQ(c.value, -3);
}

static void testAckAndReport()
{
    Command a = CommandParser::parse("ACK 7 T1");
    CHECK_EQ(a.type, CMD_ACK);
    CHECK_EQ(a.time, 7);
    CHECK_EQ(a.sensorId, std::string("T1"));

    Command r = CommandParser::parse("REPORT 9");
    CHECK_EQ(r.type, CMD_REPORT);
    CHECK_EQ(r.time, 9);
}

static void testUnknownKeyword()
{
    Command c = CommandParser::parse("RESET 3");
    CHECK_EQ(c.type, CMD_INVALID);
    CHECK_EQ(c.error, std::string("unknown command 'RESET'"));

    // Les mots-cles sont sensibles a la casse.
    CHECK_EQ(CommandParser::parse("read 1 T1 5").error, std::string("unknown command 'read'"));
}

static void testMalformed()
{
    // Mauvais nombre d'arguments
    CHECK_EQ(CommandParser::parse("SENSOR TEMP T1 X 0").error,
             std::string("malformed SENSOR command"));
    CHECK_EQ(CommandParser::parse("READ 1 T1").error, std::string("malformed READ command"));
    CHECK_EQ(CommandParser::parse("READ 1 T1 5 6").error, std::string("malformed READ command"));
    CHECK_EQ(CommandParser::parse("ACK 1").error, std::string("malformed ACK command"));
    CHECK_EQ(CommandParser::parse("REPORT").error, std::string("malformed REPORT command"));

    // Entiers invalides
    CHECK_EQ(CommandParser::parse("READ 1 T1 abc").error, std::string("malformed READ command"));
    CHECK_EQ(CommandParser::parse("READ 1 T1 12abc").error, std::string("malformed READ command"));
    CHECK_EQ(CommandParser::parse("READ x T1 12").error, std::string("malformed READ command"));
    CHECK_EQ(CommandParser::parse("SENSOR TEMP T1 X 0 1.5").error,
             std::string("malformed SENSOR command"));

    // Temps negatif interdit
    CHECK_EQ(CommandParser::parse("READ -1 T1 5").error, std::string("malformed READ command"));
    CHECK_EQ(CommandParser::parse("ACK -2 T1").error, std::string("malformed ACK command"));
    CHECK_EQ(CommandParser::parse("REPORT -3").error, std::string("malformed REPORT command"));

    CHECK_EQ(CommandParser::parse("REPORT 3 4").type, CMD_INVALID);
}

static void testParserDoesNotValidateSemantics()
{
    // Type de capteur inconnu ou plage inversee : syntaxe correcte,
    // c'est le MonitoringSystem qui refusera.
    Command c = CommandParser::parse("SENSOR VIB V1 FAN 10 5");
    CHECK_EQ(c.type, CMD_SENSOR);
    CHECK_EQ(c.sensorType, std::string("VIB"));
    CHECK_EQ(c.minValue, 10);
    CHECK_EQ(c.maxValue, 5);
}

int main()
{
    RUN_TEST(testEmptyAndComments);
    RUN_TEST(testSensor);
    RUN_TEST(testExtraSpaces);
    RUN_TEST(testAckAndReport);
    RUN_TEST(testUnknownKeyword);
    RUN_TEST(testMalformed);
    RUN_TEST(testParserDoesNotValidateSemantics);
    return checkSummary("CommandParser");
}
