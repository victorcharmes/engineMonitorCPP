#include <sstream>

#include "Logger.h"
#include "check.h"

static void testFormatInfo()
{
    std::ostringstream out;
    Logger logger(out);
    logger.log(LOG_INFO, 42, "hello");
    CHECK_EQ(out.str(), std::string("[T+0042] INFO  hello\n"));
}

static void testFormatWarningAndError()
{
    std::ostringstream out;
    Logger logger(out);
    logger.log(LOG_WARNING, 7, "attention");
    logger.log(LOG_ERROR, 1234, "panne");
    CHECK_EQ(out.str(), std::string("[T+0007] WARN  attention\n"
                                    "[T+1234] ERROR panne\n"));
}

static void testNoTime()
{
    std::ostringstream out;
    Logger logger(out);
    logger.log(LOG_ERROR, NO_TIME, "line 3: oops");
    CHECK_EQ(out.str(), std::string("[T+----] ERROR line 3: oops\n"));
}

static void testTimeZero()
{
    std::ostringstream out;
    Logger logger(out);
    logger.log(LOG_INFO, 0, "start");
    CHECK_EQ(out.str(), std::string("[T+0000] INFO  start\n"));
}

static void testFillDoesNotLeak()
{
    // Le Logger ne doit pas laisser le flux dans un etat modifie
    // (ex : caractere de remplissage '0').
    std::ostringstream out;
    Logger logger(out);
    logger.log(LOG_INFO, 1, "x");
    out.width(3);
    out << 5;
    CHECK_EQ(out.str(), std::string("[T+0001] INFO  x\n  5"));
}

static void testCounts()
{
    std::ostringstream out;
    Logger logger(out);
    CHECK_EQ(logger.count(LOG_INFO), 0);
    CHECK_EQ(logger.count(LOG_WARNING), 0);
    CHECK_EQ(logger.count(LOG_ERROR), 0);

    logger.log(LOG_INFO, 1, "a");
    logger.log(LOG_INFO, 2, "b");
    logger.log(LOG_ERROR, 3, "c");

    CHECK_EQ(logger.count(LOG_INFO), 2);
    CHECK_EQ(logger.count(LOG_WARNING), 0);
    CHECK_EQ(logger.count(LOG_ERROR), 1);
}

int main()
{
    RUN_TEST(testFormatInfo);
    RUN_TEST(testFormatWarningAndError);
    RUN_TEST(testNoTime);
    RUN_TEST(testTimeZero);
    RUN_TEST(testFillDoesNotLeak);
    RUN_TEST(testCounts);
    return checkSummary("Logger");
}
