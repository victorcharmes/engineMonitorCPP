#include "Logger.h"
#include <iomanip>

Logger::Logger(std::ostream& out)
    : m_out(out){
    for (int i = 0; i < 3; i++){
        m_counts[i] = 0;
    }
}

void Logger::log(LogLevel level, int time, const std::string& message){
    m_out << "[T+";
    if (time == NO_TIME){
        m_out << "----";
    }else{
        m_out << std::setw(4) << std::setfill('0') << time; //sur une largeur de 4 caractères, on complète de 0
        m_out << std::setfill(' '); // on remet l'espace, sinon '0' reste actif
    }
    m_out << "] ";
    if (level == LOG_INFO){
        m_out << "INFO ";
    }else if (level == LOG_WARNING){
        m_out << "WARN ";
    }else{
        m_out << "ERROR";
    }
    m_out << " " << message << "\n";
    m_counts[level]++;
}

int Logger::count(LogLevel level) const{
    return m_counts[level];
}