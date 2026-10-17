#include "PressureSensor.h"

    PressureSensor::PressureSensor(const std::string& id, const std::string& label,
                                        int minValue, int maxValue)
        : Sensor(id, label, minValue, maxValue)
    {
    }
    Severity PressureSensor::evaluate(int value) const{
        if (value > m_maxValue){
            return SEV_WARNING;
        }else if(value < m_minValue){
            return SEV_CRITICAL;
        }
        return SEV_NONE;
    }
    std::string PressureSensor::typeName() const{
        return "PRES";
    }
    std::string PressureSensor::unit() const{
        return "psi";
    }
    
    Sensor* PressureSensor::clone() const{
        return new PressureSensor(*this);
    }