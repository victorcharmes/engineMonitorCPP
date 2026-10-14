#include <string>

#include "PressureSensor.h"
#include "TemperatureSensor.h"
#include "check.h"

static void testSeverityToString()
{
    CHECK_EQ(std::string(severityToString(SEV_NONE)), std::string("NONE"));
    CHECK_EQ(std::string(severityToString(SEV_WARNING)), std::string("WARNING"));
    CHECK_EQ(std::string(severityToString(SEV_CRITICAL)), std::string("CRITICAL"));
}

static void testAccessors()
{
    TemperatureSensor t("T1", "ENG1_EGT", -50, 900);
    CHECK_EQ(t.id(), std::string("T1"));
    CHECK_EQ(t.label(), std::string("ENG1_EGT"));
    CHECK_EQ(t.minValue(), -50);
    CHECK_EQ(t.maxValue(), 900);
    CHECK(!t.hasValue());
    CHECK_EQ(t.lastValue(), 0);

    t.setValue(650);
    CHECK(t.hasValue());
    CHECK_EQ(t.lastValue(), 650);
}

static void testTemperatureRules()
{
    TemperatureSensor t("T1", "ENG1_EGT", -50, 900);
    CHECK_EQ(t.evaluate(0), SEV_NONE);
    CHECK_EQ(t.evaluate(-50), SEV_NONE);     // bornes incluses
    CHECK_EQ(t.evaluate(900), SEV_NONE);
    CHECK_EQ(t.evaluate(901), SEV_CRITICAL); // trop chaud
    CHECK_EQ(t.evaluate(-51), SEV_WARNING);  // trop froid
    CHECK_EQ(t.typeName(), std::string("TEMP"));
    CHECK_EQ(t.unit(), std::string("degC"));
}

static void testPressureRules()
{
    PressureSensor p("P1", "ENG1_OIL", 20, 80);
    CHECK_EQ(p.evaluate(50), SEV_NONE);
    CHECK_EQ(p.evaluate(20), SEV_NONE);
    CHECK_EQ(p.evaluate(80), SEV_NONE);
    CHECK_EQ(p.evaluate(19), SEV_CRITICAL);  // perte de pression
    CHECK_EQ(p.evaluate(81), SEV_WARNING);   // surpression
    CHECK_EQ(p.typeName(), std::string("PRES"));
    CHECK_EQ(p.unit(), std::string("psi"));
}

static void testDescribe()
{
    TemperatureSensor t("T1", "ENG1_EGT", -50, 900);
    PressureSensor p("P1", "ENG1_OIL", 20, 80);
    CHECK_EQ(t.describe(), std::string("T1 (ENG1_EGT) TEMP [-50..900 degC]"));
    CHECK_EQ(p.describe(), std::string("P1 (ENG1_OIL) PRES [20..80 psi]"));
}

static void testPolymorphism()
{
    // Les appels passent par un pointeur sur la classe de base.
    Sensor* sensors[2];
    sensors[0] = new TemperatureSensor("T1", "A", 0, 100);
    sensors[1] = new PressureSensor("P1", "B", 0, 100);

    CHECK_EQ(sensors[0]->evaluate(101), SEV_CRITICAL);
    CHECK_EQ(sensors[1]->evaluate(101), SEV_WARNING);
    CHECK_EQ(sensors[0]->unit(), std::string("degC"));
    CHECK_EQ(sensors[1]->unit(), std::string("psi"));

    // Destructeur virtuel : verifiable avec "make memcheck".
    delete sensors[0];
    delete sensors[1];
}

static void testClone()
{
    PressureSensor original("P1", "ENG1_OIL", 20, 80);
    original.setValue(42);

    Sensor* copy = original.clone();
    CHECK(copy != 0);
    CHECK(copy != &original);
    CHECK_EQ(copy->typeName(), std::string("PRES"));     // bon type dynamique
    CHECK_EQ(copy->describe(), original.describe());
    CHECK(copy->hasValue());
    CHECK_EQ(copy->lastValue(), 42);

    copy->setValue(10);                                   // copie independante
    CHECK_EQ(original.lastValue(), 42);
    delete copy;
}

int main()
{
    RUN_TEST(testSeverityToString);
    RUN_TEST(testAccessors);
    RUN_TEST(testTemperatureRules);
    RUN_TEST(testPressureRules);
    RUN_TEST(testDescribe);
    RUN_TEST(testPolymorphism);
    RUN_TEST(testClone);
    return checkSummary("Sensor");
}
