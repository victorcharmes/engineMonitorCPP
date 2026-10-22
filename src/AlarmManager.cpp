#include "AlarmManager.h"


Alarm::Alarm():
    sensorId(), severity(SEV_NONE), state(ALARM_ACTIVE), raisedAt(0)
{
}
Alarm::Alarm(const std::string& id, Severity sev, int time):
    sensorId(id), severity(sev), state(ALARM_ACTIVE), raisedAt(time)    
{
}

AlarmManager::AlarmManager():
    m_alarms(), m_totalRaised(0)
{
}

AlarmEvent AlarmManager::update(const std::string& sensorId, Severity severity, int time){
    AlarmMap::iterator it = m_alarms.find(sensorId);
    if(severity == SEV_NONE && !m_alarms.count(sensorId)){
        return EVT_NONE;
    }else if(severity == SEV_NONE && m_alarms.count(sensorId)){
        m_alarms.erase(sensorId);
        return EVT_CLEARED;
    }else if(severity != SEV_NONE && !m_alarms.count(sensorId)){
        m_alarms.insert(std::make_pair(sensorId, Alarm(sensorId, severity, time)));
        m_totalRaised++;
        return EVT_RAISED;
    }else if(severity != SEV_NONE && (m_alarms.count(sensorId) && it->second.severity == severity)){
        return EVT_NONE;
    }else{
        it->second.severity = severity;
        it->second.state = ALARM_ACTIVE;
        it->second.raisedAt = time;
        return EVT_UPDATED;
    }
}

bool AlarmManager::acknowledge(const std::string& sensorId){
    AlarmMap::iterator it = m_alarms.find(sensorId);
    if (it == m_alarms.end() || it->second.state == ALARM_ACKNOWLEDGED){   
        return false;
    }else{
        it->second.state = ALARM_ACKNOWLEDGED;
        return true;
    }
}
const Alarm* AlarmManager::find(const std::string& sensorId) const{
    AlarmMap::const_iterator it = m_alarms.find(sensorId);
    if (it == m_alarms.end()){
        return 0;
    }else{
        return &it->second;
    }
}
std::size_t AlarmManager::count() const{
    return m_alarms.size();
}
std::size_t AlarmManager::unacknowledgedCount() const{
    AlarmMap::const_iterator it = m_alarms.begin();
    size_t nbAc = 0;
    while (it != m_alarms.end()){
        if(it->second.state == ALARM_ACTIVE){
            nbAc++;
        }
        it++;
    }
    return nbAc;
}

int AlarmManager::totalRaised() const{
    return m_totalRaised;
}