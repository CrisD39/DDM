#pragma once

#include <QString>
#include "model/cpa.h"

class CommandContext;
class Track;

/**
 * @brief Servicio para cálculo de CPA (Closest Point of Approach)
 * 
 * Encapsula la lógica de búsqueda de tracks y cálculo de punto de aproximación mínima,
 * ocultando detalles del algoritmo CPA al usuario del servicio.
 */
class CPAService {
public:
    /**
     * @brief Constructor que inyecta el contexto
     * @param context Puntero a CommandContext (no nulo)
     */
    explicit CPAService(CommandContext* context);

    /**
     * @brief Calcula CPA entre dos tracks
     * @param trackId1 ID del primer track
     * @param trackId2 ID del segundo track
     * @return CPAResult con TCPA (segundos) y DCPA (Data Miles)
     * @note Si un track no existe, retorna CPAResult.valid = false
     * @note TCPA < 0 significa tracks divergen; TCPA >= 0 significa convergencia
     */
    CPAResult computeCPA(int trackId1, int trackId2) const;

private:
    CommandContext* m_context;
};
