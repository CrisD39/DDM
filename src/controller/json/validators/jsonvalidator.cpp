#include "jsonvalidator.h"
#include <QJsonValue>

ValidationResult JsonValidator::validateNumericField(
    const QJsonObject& args,
    const QString& fieldName,
    double minValue,
    double maxValue,
    bool isRequired,
    double defaultValue)
{
    QJsonValue value = args.value(fieldName);
    
    // Si el campo no existe
    if (value.isUndefined() || value.isNull()) {
        if (isRequired) {
            return ValidationResult::error(
                "MISSING_REQUIRED_FIELDS",
                QString("El campo '%1' es requerido y debe ser un número").arg(fieldName),
                fieldName,
                "",
                QString("%1 - %2").arg(minValue).arg(maxValue)
            );
        }
        return ValidationResult::success();
    }
    
    // Si el campo no es numérico
    if (!value.isDouble()) {
        return ValidationResult::error(
            "INVALID_FIELD_TYPE",
            QString("El campo '%1' debe ser un número").arg(fieldName),
            fieldName,
            value.toString(),
            QString("%1 - %2").arg(minValue).arg(maxValue)
        );
    }
    
    double numValue = value.toDouble();
    
    // Validar rango
    if (numValue < minValue || numValue > maxValue) {
        return ValidationResult::error(
            "VALIDATION_ERROR",
            QString("El campo '%1' está fuera de rango").arg(fieldName),
            fieldName,
            QString::number(numValue),
            QString("%1 - %2").arg(minValue).arg(maxValue)
        );
    }
    
    return ValidationResult::success();
}

ValidationResult JsonValidator::validateIntegerField(
    const QJsonObject& args,
    const QString& fieldName,
    int minValue,
    int maxValue,
    bool isRequired,
    int defaultValue)
{
    QJsonValue value = args.value(fieldName);
    
    // Si el campo no existe
    if (value.isUndefined() || value.isNull()) {
        if (isRequired) {
            return ValidationResult::error(
                "MISSING_REQUIRED_FIELDS",
                QString("El campo '%1' es requerido y debe ser un entero").arg(fieldName),
                fieldName,
                "",
                QString("%1 - %2").arg(minValue).arg(maxValue)
            );
        }
        return ValidationResult::success();
    }
    
    // Si el campo no es numérico
    if (!value.isDouble()) {
        return ValidationResult::error(
            "INVALID_FIELD_TYPE",
            QString("El campo '%1' debe ser un entero").arg(fieldName),
            fieldName,
            value.toString(),
            QString("%1 - %2").arg(minValue).arg(maxValue)
        );
    }
    
    int intValue = value.toInt();
    
    // Validar rango
    if (intValue < minValue || intValue > maxValue) {
        return ValidationResult::error(
            "VALIDATION_ERROR",
            QString("El campo '%1' está fuera de rango").arg(fieldName),
            fieldName,
            QString::number(intValue),
            QString("%1 - %2").arg(minValue).arg(maxValue)
        );
    }
    
    return ValidationResult::success();
}

ValidationResult JsonValidator::validateStringField(
    const QJsonObject& args,
    const QString& fieldName,
    bool isRequired)
{
    QJsonValue value = args.value(fieldName);
    
    if (!value.isString() || value.toString().isEmpty()) {
        if (isRequired) {
            return ValidationResult::error(
                "MISSING_REQUIRED_FIELDS",
                QString("El campo '%1' es requerido y debe ser un string no vacío").arg(fieldName),
                fieldName
            );
        }
    }
    
    return ValidationResult::success();
}

ValidationResult JsonValidator::validateLineIdFormat(const QString& lineId)
{
    if (!lineId.startsWith("LINE_")) {
        return ValidationResult::error(
            "INVALID_ID_FORMAT",
            QString("Formato de ID inválido: %1. Esperado: LINE_<número>").arg(lineId),
            "id",
            lineId,
            "LINE_<número>"
        );
    }
    
    bool ok;
    lineId.mid(5).toInt(&ok);
    
    if (!ok) {
        return ValidationResult::error(
            "INVALID_ID_FORMAT",
            QString("No se pudo extraer número del ID: %1").arg(lineId),
            "id",
            lineId
        );
    }
    
    return ValidationResult::success();
}

double JsonValidator::getNumericValue(const QJsonObject& args, const QString& fieldName, double defaultValue)
{
    return args.value(fieldName).toDouble(defaultValue);
}

int JsonValidator::getIntValue(const QJsonObject& args, const QString& fieldName, int defaultValue)
{
    return args.value(fieldName).toInt(defaultValue);
}
