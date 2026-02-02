#pragma once

#include <QJsonArray>
#include <QJsonObject>

class CursorEntity;

/**
 * @brief Helper para serializar entidades del modelo a formato JSON
 * 
 * Centraliza la lógica de conversión de objetos del dominio a JSON,
 * evitando duplicación de código en los handlers de comandos.
 */
class JsonSerializer
{
public:
    /**
     * @brief Serializa un cursor/línea a un objeto JSON
     * @param cursor Referencia a la entidad cursor
     * @return QJsonObject con la representación JSON de la línea
     */
    static QJsonObject serializeLine(const CursorEntity& cursor);
    
    /**
     * @brief Serializa una colección de cursores a un array JSON
     * @tparam Container Cualquier contenedor iterable (std::deque, QList, std::vector, etc.)
     * @param cursors Colección de cursores a serializar
     * @return QJsonArray con todas las líneas serializadas
     */
    template <typename Container>
    static QJsonArray serializeLineList(const Container& cursors)
    {
        QJsonArray linesArray;
        
        for (const auto& cursor : cursors) {
            linesArray.append(serializeLine(cursor));
        }
        
        return linesArray;
    }
};
