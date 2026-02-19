#pragma once
#include <QString>

struct StationEntity {
    int slotId;         // 1 a 10
    int trackA;         // Buque Propio [cite: 249]
    int trackB;         // Buque Guía [cite: 253]
    double azimut;      // Relativo [cite: 254]
    double distancia;   // Millas [cite: 254]

    // Resultados calculados (lo que se muestra en amarillo en la GUI )
    double rumboResultado;
    double tiempoResultado; // En segundos
    double velocidadRequerida; // Si fue por modo DU
    bool esModoVD;         // true = Velocidad, false = Tiempo [cite: 255]

    // Constructor simple
    StationEntity(int s=1, int tA=0, int tB=0, double az=0, double dt=0)
        : slotId(s), trackA(tA), trackB(tB), azimut(az), distancia(dt),
        rumboResultado(0), tiempoResultado(0), velocidadRequerida(0), esModoVD(true) {}
};
