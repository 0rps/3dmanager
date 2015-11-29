#include "army.h"

#include "db.h"

QHash<int,QString> staticAllArmies;

Army::Army():
    m_isRussian(true),
    m_id(-1),
    m_type(AT_INVALID)
{
}

int Army::id() const
{
    return m_id;
}


void Army::setRussian(const bool _isRussian)
{
    m_isRussian = _isRussian;
}

Army Army::getArmy(const int _typeId)
{
    return getArmy((ArmyType)_typeId);
}

Army Army::getArmy(const ArmyType _type)
{
    return m_armies.value(_type, Army);
}


bool Army::isRussian() const
{
    return m_isRussian;
}


void Army::setName(const QString &_name)
{
    m_name = _name;
}


QString Army::name() const
{
    return m_name;
}


void Army::setArmyType(const ArmyType _type) const
{
    m_type = _type;
}


bool Army::isValidArmyType() const
{
    return m_type != AT_INVALID && m_type != AT_UNKNOWN;
}


ArmyType Army::type() const
{
    return m_type;
}


void Army::setId(const int _id)
{
    m_id = _id;
}


bool Army::isValid() const
{
    return m_id > 0;
}


void Army::updateArmiesInternally()
{
    m_armies.clear();
    foreach(const Army &_army, Db::getArmies())
    {
        m_armies.insert(_army.type(), _army);
    }
}
