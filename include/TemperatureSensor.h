#ifndef MONITOR_SENSOR_TEMPERATURE_H
#define MONITOR_SENSOR_TEMPERATURE_H
#include "Sensor.h"

class TemperatureSensor : public Sensor
{
public:
    TemperatureSensor(const std::string& id, const std::string& label, int minValue, int maxValue);

    virtual Severity evaluate(int value) const;
    virtual std::string typeName() const;
    virtual std::string unit() const;
    virtual Sensor* clone() const;
};

#endif