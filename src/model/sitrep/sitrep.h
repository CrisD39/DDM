#pragma once

#include <QString>
#include <QDateTime>
#include <QMetaType>

// sitrep = situation report
// modelo puro (datos + validación mínima)
class sitrep
{
public:
    enum class category {
        unknown = 0,
        surface,
        air,
        subsurface,
        ew,
        weapons,
        system
    };

    sitrep() = default;
    sitrep(const QString& title, const QString& body);

    // identidad (opcional)
    QString id() const;
    void setId(const QString& id);

    // categoría / ambiente
    category getCategory() const;
    void setCategory(category c);

    // contenido
    QString title() const;
    void setTitle(const QString& t);

    QString body() const;
    void setBody(const QString& b);

    // marca temporal (utc recomendado)
    QDateTime timestampUtc() const;
    void setTimestampUtc(const QDateTime& tsUtc);

    // origen (operador, sistema, módulo, etc.)
    QString source() const;
    void setSource(const QString& src);

    // validación mínima
    bool isValid() const;

private:
    QString  m_id;
    category m_category { category::unknown };

    QString  m_title;
    QString  m_body;

    QDateTime m_timestampUtc;
    QString   m_source;
};

Q_DECLARE_METATYPE(sitrep)
