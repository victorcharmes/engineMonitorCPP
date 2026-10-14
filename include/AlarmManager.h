#ifndef MONITOR_ALARM_MANAGER_H
#define MONITOR_ALARM_MANAGER_H

#include <cstddef>
#include <map>
#include <string>

#include "Sensor.h"   // Severity

enum AlarmState
{
    ALARM_ACTIVE,        // levee, pas encore acquittee
    ALARM_ACKNOWLEDGED   // acquittee par l'operateur, toujours presente
};

/// Une alarme en cours sur un capteur (au plus une par capteur).
struct Alarm
{
    std::string sensorId;
    Severity severity;
    AlarmState state;
    int raisedAt;   // temps de la levee (ou de la derniere mise a jour)

    Alarm();
    Alarm(const std::string& id, Severity sev, int time);
};

/// Ce qui s'est passe lors d'un appel a AlarmManager::update().
enum AlarmEvent
{
    EVT_NONE,      // rien n'a change
    EVT_RAISED,    // nouvelle alarme
    EVT_UPDATED,   // alarme existante dont la gravite a change
    EVT_CLEARED    // alarme supprimee (retour a la normale)
};

class AlarmManager
{
public:
    AlarmManager();

    /// Applique la gravite de la derniere mesure d'un capteur.
    /// Regles detaillees dans le sujet (etape 4).
    AlarmEvent update(const std::string& sensorId, Severity severity, int time);

    /// Acquitte l'alarme du capteur.
    /// false s'il n'y a pas d'alarme ou si elle est deja acquittee.
    bool acknowledge(const std::string& sensorId);

    /// 0 s'il n'y a pas d'alarme pour ce capteur.
    const Alarm* find(const std::string& sensorId) const;

    /// Nombre d'alarmes presentes (actives + acquittees).
    std::size_t count() const;
    /// Nombre d'alarmes a l'etat ALARM_ACTIVE.
    std::size_t unacknowledgedCount() const;
    /// Nombre total d'alarmes levees depuis la creation (EVT_RAISED).
    int totalRaised() const;

private:   // --- partie privée : libre, tu peux la modifier ---
    typedef std::map<std::string, Alarm> AlarmMap;

    AlarmMap m_alarms;
    int m_totalRaised;
};

#endif
