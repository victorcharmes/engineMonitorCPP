#ifndef MONITOR_SENSOR_H
#define MONITOR_SENSOR_H

#include <string>

/// Gravite d'une mesure.
enum Severity
{
    SEV_NONE = 0,
    SEV_WARNING = 1,
    SEV_CRITICAL = 2
};

/// Renvoie "NONE", "WARNING" ou "CRITICAL".
const char* severityToString(Severity severity);

/// Classe de base abstraite de tous les capteurs.
///
/// Un capteur a un identifiant unique (ex : "T1"), un libelle (ex : "ENG1_EGT"),
/// une plage nominale [minValue, maxValue] (bornes incluses) et memorise
/// sa derniere valeur lue.
class Sensor
{
public:
    Sensor(const std::string& id, const std::string& label, int minValue, int maxValue);
    virtual ~Sensor();

    const std::string& id() const;
    const std::string& label() const;
    int minValue() const;
    int maxValue() const;

    /// false tant qu'aucune valeur n'a ete enregistree.
    bool hasValue() const;
    /// Derniere valeur enregistree (0 si hasValue() == false).
    int lastValue() const;
    void setValue(int value);

    /// Gravite d'une valeur pour ce type de capteur (regles dans le sujet).
    virtual Severity evaluate(int value) const = 0;
    /// Mot-cle du type : "TEMP", "PRES"...
    virtual std::string typeName() const = 0;
    /// Unite d'affichage : "degC", "psi"...
    virtual std::string unit() const = 0;
    /// Copie profonde polymorphe (allouee avec new, a liberer par l'appelant).
    virtual Sensor* clone() const = 0;

    /// Ex : "T1 (ENG1_EGT) TEMP [-50..900 degC]"
    std::string describe() const;

protected:
    std::string m_id;
    std::string m_label;
    int m_minValue;
    int m_maxValue;
    bool m_hasValue;
    int m_lastValue;
};

#endif
