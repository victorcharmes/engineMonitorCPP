#include <string>
#include <vector>

#include "PressureSensor.h"
#include "SensorRegistry.h"
#include "TemperatureSensor.h"
#include "check.h"

static void testEmpty()
{
    SensorRegistry reg;
    CHECK_EQ(reg.size(), 0u);
    CHECK(reg.find("T1") == 0);
    CHECK(reg.ids().empty());
}

static void testAddAndFind()
{
    SensorRegistry reg;
    CHECK(reg.add(new TemperatureSensor("T1", "ENG1_EGT", -50, 900)));
    CHECK(reg.add(new PressureSensor("P1", "ENG1_OIL", 20, 80)));
    CHECK_EQ(reg.size(), 2u);

    Sensor* t1 = reg.find("T1");
    CHECK(t1 != 0);
    if (t1 != 0)
    {
        CHECK_EQ(t1->typeName(), std::string("TEMP"));
    }
    CHECK(reg.find("X9") == 0);

    const SensorRegistry& constReg = reg;
    CHECK(constReg.find("P1") != 0);
}

static void testDuplicate()
{
    SensorRegistry reg;
    CHECK(reg.add(new TemperatureSensor("T1", "FIRST", 0, 10)));
    // Doublon : refuse, et le capteur passe en parametre est detruit
    // par add() (sinon fuite memoire -> "make memcheck").
    CHECK(!reg.add(new PressureSensor("T1", "SECOND", 0, 10)));
    CHECK_EQ(reg.size(), 1u);
    CHECK_EQ(reg.find("T1")->label(), std::string("FIRST"));
}

static void testNullPointer()
{
    SensorRegistry reg;
    CHECK(!reg.add(0));
    CHECK_EQ(reg.size(), 0u);
}

static void testIdsSorted()
{
    SensorRegistry reg;
    reg.add(new TemperatureSensor("T2", "A", 0, 1));
    reg.add(new PressureSensor("P1", "B", 0, 1));
    reg.add(new TemperatureSensor("T1", "C", 0, 1));

    std::vector<std::string> ids = reg.ids();
    CHECK_EQ(ids.size(), 3u);
    if (ids.size() == 3)
    {
        CHECK_EQ(ids[0], std::string("P1"));
        CHECK_EQ(ids[1], std::string("T1"));
        CHECK_EQ(ids[2], std::string("T2"));
    }
}

static void testCopyConstructorIsDeep()
{
    SensorRegistry original;
    original.add(new TemperatureSensor("T1", "ENG1_EGT", -50, 900));
    original.find("T1")->setValue(100);

    SensorRegistry copy(original);
    CHECK_EQ(copy.size(), 1u);
    CHECK(copy.find("T1") != 0);
    CHECK(copy.find("T1") != original.find("T1"));       // objets distincts
    CHECK_EQ(copy.find("T1")->typeName(), std::string("TEMP"));
    CHECK_EQ(copy.find("T1")->lastValue(), 100);

    copy.find("T1")->setValue(500);
    CHECK_EQ(original.find("T1")->lastValue(), 100);     // original intact
}

static void testAssignment()
{
    SensorRegistry a;
    a.add(new TemperatureSensor("T1", "A", 0, 10));
    a.add(new PressureSensor("P1", "A", 0, 10));

    SensorRegistry b;
    b.add(new PressureSensor("OLD", "B", 0, 10));   // doit etre libere par l'affectation

    b = a;
    CHECK_EQ(b.size(), 2u);
    CHECK(b.find("OLD") == 0);
    CHECK(b.find("T1") != 0);
    CHECK(b.find("T1") != a.find("T1"));
    CHECK_EQ(b.find("P1")->typeName(), std::string("PRES"));
}

static void testSelfAssignment()
{
    SensorRegistry a;
    a.add(new TemperatureSensor("T1", "A", 0, 10));
    a.find("T1")->setValue(7);

    SensorRegistry& alias = a;
    a = alias;   // ne doit ni planter ni perdre de donnees
    CHECK_EQ(a.size(), 1u);
    CHECK(a.find("T1") != 0);
    if (a.find("T1") != 0)
    {
        CHECK_EQ(a.find("T1")->lastValue(), 7);
    }
}

static void testCopyIsolationOnDestruction()
{
    SensorRegistry* original = new SensorRegistry();
    original->add(new TemperatureSensor("T1", "A", 0, 10));

    SensorRegistry copy(*original);
    delete original;   // ne doit pas detruire les capteurs de 'copy'

    CHECK(copy.find("T1") != 0);
    if (copy.find("T1") != 0)
    {
        CHECK_EQ(copy.find("T1")->label(), std::string("A"));
    }
}

int main()
{
    RUN_TEST(testEmpty);
    RUN_TEST(testAddAndFind);
    RUN_TEST(testDuplicate);
    RUN_TEST(testNullPointer);
    RUN_TEST(testIdsSorted);
    RUN_TEST(testCopyConstructorIsDeep);
    RUN_TEST(testAssignment);
    RUN_TEST(testSelfAssignment);
    RUN_TEST(testCopyIsolationOnDestruction);
    return checkSummary("SensorRegistry");
}
