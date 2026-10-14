#ifndef MONITOR_MONITORING_SYSTEM_H
#define MONITOR_MONITORING_SYSTEM_H

#include <istream>
#include <ostream>
#include <string>

#include "AlarmManager.h"
#include "CommandParser.h"
#include "Logger.h"
#include "SensorRegistry.h"

/// Orchestrateur : lit un scenario ligne par ligne, et pour chaque ligne
/// l'analyse (CommandParser), l'execute (SensorRegistry, AlarmManager)
/// et journalise le resultat (Logger).
class MonitoringSystem
{
public:
    explicit MonitoringSystem(std::ostream& out);

    /// Traite tout le flux puis ecrit la ligne de fin de traitement.
    /// Renvoie le nombre de lignes ERROR ecrites.
    int run(std::istream& in);

    /// Traite une seule ligne (lineNumber commence a 1).
    void processLine(const std::string& line, int lineNumber);

    const SensorRegistry& sensors() const;
    const AlarmManager& alarms() const;

private:   // --- partie privée : libre, tu peux la modifier ---
    MonitoringSystem(const MonitoringSystem&);
    MonitoringSystem& operator=(const MonitoringSystem&);

    void handleSensor(const Command& cmd, int lineNumber);
    void handleRead(const Command& cmd, int lineNumber);
    void handleAck(const Command& cmd, int lineNumber);
    void handleReport(const Command& cmd);
    void logError(int time, int lineNumber, const std::string& message);

    /// Fabrique : renvoie 0 si le type est inconnu.
    static Sensor* createSensor(const Command& cmd);

    Logger m_logger;
    SensorRegistry m_registry;
    AlarmManager m_alarms;
};

#endif
