#ifndef MONITOR_SENSOR_PRESSURE_H
#define MONITOR_SENSOR_PRESSURE_H
#include "Sensor.h"

class PressureSensor : public Sensor
{
public:
    PressureSensor(const std::string& id, const std::string& label, int minValue, int maxValue);

    virtual Severity evaluate(int value) const;
    virtual std::string typeName() const;
    virtual std::string unit() const;
    virtual Sensor* clone() const;
};

#endif