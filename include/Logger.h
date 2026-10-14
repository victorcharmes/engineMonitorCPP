#ifndef MONITOR_LOGGER_H
#define MONITOR_LOGGER_H

#include <ostream>
#include <string>

/// Niveaux de journalisation.
enum LogLevel
{
    LOG_INFO = 0,
    LOG_WARNING = 1,
    LOG_ERROR = 2
};

/// Valeur de temps a passer quand l'evenement n'a pas d'horodatage.
const int NO_TIME = -1;

/// Ecrit des lignes de journal formatees sur un flux de sortie.
///
/// Format d'une ligne :
///   [T+0042] INFO  message
///   [T+0042] WARN  message
///   [T+----] ERROR message        (quand time == NO_TIME)
///
/// - le temps est affiche sur 4 chiffres, complete par des zeros ;
/// - le niveau occupe toujours 5 caracteres ("INFO ", "WARN ", "ERROR") ;
/// - un espace separe le niveau du message ; chaque ligne finit par '\n'.
class Logger
{
public:
    explicit Logger(std::ostream& out);

    void log(LogLevel level, int time, const std::string& message);

    /// Nombre de lignes deja ecrites pour ce niveau.
    int count(LogLevel level) const;

private:   // --- partie privée : libre, tu peux la modifier ---
    // Non copiable : declares prives et volontairement non definis.
    Logger(const Logger&);
    Logger& operator=(const Logger&);

    std::ostream& m_out;
    int m_counts[3];
};

#endif
