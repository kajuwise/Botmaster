#include "stateholder.h"

StateHolder::StateHolder(char const* name, Neve::State state)
    : m_state(state)
    , m_name(name)
{
    //logState();
}

char* StateHolder::toString() const
{
    switch (m_state)
    {
        case Neve::WAITING_START: return "WAITING_START"; //1
        case Neve::FIND_BALL: return "FIND_BALL"; //2
        case Neve::IMPROVE_CONTACT_WITH_BALL: return "IMPROVE_CONTACT_WITH_BALL"; //3
        case Neve::ESCAPE_BLACK_LINE: return "ESCAPE_BLACK_LINE";
        case Neve::START_ESCAPE_BLACK_LINE: return "START_ESCAPE_BLACK_LINE";
        case Neve::DISTRONIC_TURN: return "DISTRONIC_TURN";
        case Neve::ESCAPE_GOAL_TOO_CLOSE: return "GOAL_TOO_CLOSE";
        case Neve::THROW: return "THROW";

    }
}

StateHolder & StateHolder::operator= (const Neve::State other)
{
    if (this->m_state != other) // protect against invalid self-assignment
    {
        m_state = other;
        logState();
    }

    return *this;
}

void StateHolder::logState() const
{
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << m_name << "| new state: " << toString();
}

