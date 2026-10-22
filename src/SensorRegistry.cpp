#include "SensorRegistry.h"
#include "Sensor.h"
#include <map>

SensorRegistry::SensorRegistry(){}

SensorRegistry::SensorRegistry(const SensorRegistry& other)
{
    copyFrom(other);
}

SensorRegistry& SensorRegistry::operator=(const SensorRegistry& other){
    if (this != &other){
        clear();
        copyFrom(other);
    }
    return *this;
}

SensorRegistry::~SensorRegistry(){
    clear();
}

bool SensorRegistry::add(Sensor* sensor){
    if(!sensor){
        return false;
    }
    else if (m_sensors.count(sensor->id())){
        delete sensor;
        return false;
    }else{
        m_sensors.insert(std::make_pair(sensor->id(), sensor));
        return true;
    }
}

Sensor* SensorRegistry::find(const std::string& id){
    SensorMap::iterator it = m_sensors.find(id);
    if (it != m_sensors.end()){
        return it->second;
    }else{
        return 0;
    }
}

const Sensor* SensorRegistry::find(const std::string& id) const{
    SensorMap::const_iterator it = m_sensors.find(id);
    if (it != m_sensors.end()){
        return it->second;
    }else{
        return 0;
    }
}

std::size_t SensorRegistry::size() const{
    return m_sensors.size();
}

std::vector<std::string> SensorRegistry::ids() const{
    std::vector<std::string> retour;
    for (SensorMap::const_iterator it = m_sensors.begin(); it != m_sensors.end(); ++it){
        retour.push_back(it->first);
    }
    return retour;
}

void SensorRegistry::clear(){
    for (SensorMap::const_iterator it = m_sensors.begin(); it != m_sensors.end(); ++it){
        delete it->second;
    }
    m_sensors.clear();
}

void SensorRegistry::copyFrom(const SensorRegistry& other){
    for (SensorMap::const_iterator it = other.m_sensors.begin(); it != other.m_sensors.end(); ++it){
        m_sensors.insert(std::make_pair(it->first, it->second->clone()));
    }
}