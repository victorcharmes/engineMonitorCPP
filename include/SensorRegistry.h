#ifndef MONITOR_SENSOR_REGISTRY_H
#define MONITOR_SENSOR_REGISTRY_H

#include <cstddef>
#include <map>
#include <string>
#include <vector>

class Sensor;

/// Conteneur proprietaire des capteurs, indexes par identifiant.
///
/// Le registre POSSEDE les capteurs qu'on lui confie : il les detruit
/// lui-meme. Il est copiable : une copie possede ses propres capteurs
/// (copie profonde), totalement independants de ceux de l'original.
class SensorRegistry
{
public:
    SensorRegistry();
    SensorRegistry(const SensorRegistry& other);
    SensorRegistry& operator=(const SensorRegistry& other);
    ~SensorRegistry();

    /// Prend possession de 'sensor'.
    /// Renvoie false si un capteur de meme id existe deja : dans ce cas
    /// 'sensor' est detruit immediatement et le registre est inchange.
    bool add(Sensor* sensor);

    /// Renvoie 0 si l'id est inconnu. Le registre reste proprietaire.
    Sensor* find(const std::string& id);
    const Sensor* find(const std::string& id) const;

    std::size_t size() const;

    /// Identifiants tries par ordre alphabetique.
    std::vector<std::string> ids() const;

private:   // --- partie privée : libre, tu peux la modifier ---
    typedef std::map<std::string, Sensor*> SensorMap;

    void clear();
    void copyFrom(const SensorRegistry& other);

    SensorMap m_sensors;
};

#endif
