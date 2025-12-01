#pragma once

#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QPair>

/**
 * @brief Resultado de una validación
 */
struct ValidationResult
{
    bool isValid;
    QString errorCode;
    QString errorMessage;
    QString fieldName;
    QString fieldValue;
    QString expectedRange;
    
    ValidationResult() : isValid(true) {}
    
    static ValidationResult success() {
        return ValidationResult();
    }
    
    static ValidationResult error(const QString& code, const QString& message,
                                 const QString& field = "", const QString& value = "",
                                 const QString& range = "") {
        ValidationResult result;
        result.isValid = false;
        result.errorCode = code;
        result.errorMessage = message;
        result.fieldName = field;
        result.fieldValue = value;
        result.expectedRange = range;
        return result;
    }
};

/**
 * @brief Validador de campos JSON para comandos
 * Aplica Single Responsibility Principle separando validación de lógica de negocio
 */
class JsonValidator
{
public:
    /**
     * @brief Valida que un campo numérico exista y esté en el rango especificado
     */
    static ValidationResult validateNumericField(
        const QJsonObject& args,
        const QString& fieldName,
        double minValue,
        double maxValue,
        bool isRequired = true,
        double defaultValue = 0.0
    );
    
    /**
     * @brief Valida que un campo entero exista y esté en el rango especificado
     */
    static ValidationResult validateIntegerField(
        const QJsonObject& args,
        const QString& fieldName,
        int minValue,
        int maxValue,
        bool isRequired = true,
        int defaultValue = 0
    );
    
    /**
     * @brief Valida que un campo string exista y no esté vacío
     */
    static ValidationResult validateStringField(
        const QJsonObject& args,
        const QString& fieldName,
        bool isRequired = true
    );
    
    /**
     * @brief Valida el formato de un ID de línea (LINE_<número>)
     */
    static ValidationResult validateLineIdFormat(const QString& lineId);
    
    /**
     * @brief Extrae un valor numérico con default
     */
    static double getNumericValue(const QJsonObject& args, const QString& fieldName, double defaultValue = 0.0);
    
    /**
     * @brief Extrae un valor entero con default
     */
    static int getIntValue(const QJsonObject& args, const QString& fieldName, int defaultValue = 0);
};
