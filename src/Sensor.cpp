#include "Sensor.h"
#include <sstream>

const char* severityToString(Severity severity){
    switch (severity){
        case SEV_NONE: return "NONE";
        case SEV_WARNING: return "WARNING";
        case SEV_CRITICAL: return "CRITICAL";
    }
    return "INCONNU";
}


Sensor::Sensor(const std::string& id, const std::string& label, int minValue, int maxValue)
    : m_id(id), m_label(label), m_minValue(minValue), m_maxValue(maxValue),
    m_hasValue(false), m_lastValue(0)
{
}

Sensor::~Sensor(){}

const std::string& Sensor::id() const{
    return m_id;
}

const std::string& Sensor::label() const{
    return m_label;
}

int Sensor::minValue() const{
    return m_minValue;
}

int Sensor::maxValue() const{
    return m_maxValue;
}

bool Sensor::hasValue() const{
    return (m_hasValue);
}

int Sensor::lastValue() const{
    return m_lastValue;
}

void Sensor::setValue(int value){
    m_lastValue = value;
    m_hasValue = true;
}

/// Ex : "T1 (ENG1_EGT) TEMP [-50..900 degC]"
std::string Sensor::describe() const{
    std::ostringstream oss;
    oss << m_id <<  " (" << m_label << ") " << typeName() << " [" << m_minValue << ".." << m_maxValue << " " << unit() << "]";
    return oss.str();
}
