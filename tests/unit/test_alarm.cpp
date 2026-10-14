#include "AlarmManager.h"
#include "check.h"

static void testInitialState()
{
    AlarmManager mgr;
    CHECK_EQ(mgr.count(), 0u);
    CHECK_EQ(mgr.unacknowledgedCount(), 0u);
    CHECK_EQ(mgr.totalRaised(), 0);
    CHECK(mgr.find("T1") == 0);
}

static void testNormalValueWithoutAlarm()
{
    AlarmManager mgr;
    CHECK_EQ(mgr.update("T1", SEV_NONE, 1), EVT_NONE);
    CHECK_EQ(mgr.count(), 0u);
}

static void testRaise()
{
    AlarmManager mgr;
    CHECK_EQ(mgr.update("T1", SEV_CRITICAL, 5), EVT_RAISED);
    CHECK_EQ(mgr.count(), 1u);
    CHECK_EQ(mgr.unacknowledgedCount(), 1u);
    CHECK_EQ(mgr.totalRaised(), 1);

    const Alarm* a = mgr.find("T1");
    CHECK(a != 0);
    if (a != 0)
    {
        CHECK_EQ(a->sensorId, std::string("T1"));
        CHECK_EQ(a->severity, SEV_CRITICAL);
        CHECK_EQ(a->state, ALARM_ACTIVE);
        CHECK_EQ(a->raisedAt, 5);
    }
}

static void testSameSeverityIsNoop()
{
    AlarmManager mgr;
    mgr.update("T1", SEV_WARNING, 1);
    CHECK_EQ(mgr.update("T1", SEV_WARNING, 2), EVT_NONE);
    CHECK_EQ(mgr.totalRaised(), 1);
    CHECK_EQ(mgr.find("T1")->raisedAt, 1);
}

static void testUpdateSeverity()
{
    AlarmManager mgr;
    mgr.update("T1", SEV_WARNING, 1);
    CHECK_EQ(mgr.update("T1", SEV_CRITICAL, 3), EVT_UPDATED);
    CHECK_EQ(mgr.count(), 1u);
    CHECK_EQ(mgr.totalRaised(), 1);              // une mise a jour n'est pas une levee
    CHECK_EQ(mgr.find("T1")->severity, SEV_CRITICAL);
    CHECK_EQ(mgr.find("T1")->raisedAt, 3);
}

static void testUpdateResetsAcknowledgement()
{
    AlarmManager mgr;
    mgr.update("T1", SEV_WARNING, 1);
    CHECK(mgr.acknowledge("T1"));
    CHECK_EQ(mgr.unacknowledgedCount(), 0u);

    mgr.update("T1", SEV_CRITICAL, 2);           // gravite changee -> re-acquittement requis
    CHECK_EQ(mgr.find("T1")->state, ALARM_ACTIVE);
    CHECK_EQ(mgr.unacknowledgedCount(), 1u);
}

static void testClear()
{
    AlarmManager mgr;
    mgr.update("T1", SEV_CRITICAL, 1);
    CHECK_EQ(mgr.update("T1", SEV_NONE, 2), EVT_CLEARED);
    CHECK_EQ(mgr.count(), 0u);
    CHECK(mgr.find("T1") == 0);
    CHECK_EQ(mgr.totalRaised(), 1);              // l'historique est conserve

    CHECK_EQ(mgr.update("T1", SEV_WARNING, 3), EVT_RAISED);
    CHECK_EQ(mgr.totalRaised(), 2);
}

static void testAcknowledge()
{
    AlarmManager mgr;
    CHECK(!mgr.acknowledge("T1"));               // pas d'alarme

    mgr.update("T1", SEV_WARNING, 1);
    CHECK(mgr.acknowledge("T1"));
    CHECK_EQ(mgr.find("T1")->state, ALARM_ACKNOWLEDGED);
    CHECK(!mgr.acknowledge("T1"));               // deja acquittee
    CHECK_EQ(mgr.count(), 1u);                   // acquittee != supprimee
}

static void testSeveralSensors()
{
    AlarmManager mgr;
    mgr.update("T1", SEV_WARNING, 1);
    mgr.update("P1", SEV_CRITICAL, 1);
    mgr.update("P2", SEV_NONE, 1);
    mgr.acknowledge("P1");

    CHECK_EQ(mgr.count(), 2u);
    CHECK_EQ(mgr.unacknowledgedCount(), 1u);
    CHECK_EQ(mgr.totalRaised(), 2);
}

int main()
{
    RUN_TEST(testInitialState);
    RUN_TEST(testNormalValueWithoutAlarm);
    RUN_TEST(testRaise);
    RUN_TEST(testSameSeverityIsNoop);
    RUN_TEST(testUpdateSeverity);
    RUN_TEST(testUpdateResetsAcknowledgement);
    RUN_TEST(testClear);
    RUN_TEST(testAcknowledge);
    RUN_TEST(testSeveralSensors);
    return checkSummary("AlarmManager");
}
