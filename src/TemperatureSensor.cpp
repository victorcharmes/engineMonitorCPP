#include "TemperatureSensor.h"

    TemperatureSensor::TemperatureSensor(const std::string& id, const std::string& label,
                                        int minValue, int maxValue)
        : Sensor(id, label, minValue, maxValue)
    {
    }
    Severity TemperatureSensor::evaluate(int value) const{
        if (value > m_maxValue){
            return SEV_CRITICAL;
        }else if(value < m_minValue){
            return SEV_WARNING;
        }
        return SEV_NONE;
    }
    std::string TemperatureSensor::typeName() const{
        return "TEMP";
    }
    std::string TemperatureSensor::unit() const{
        return "degC";
    }
    Sensor* TemperatureSensor::clone() const{
        return new TemperatureSensor(*this);
    }
