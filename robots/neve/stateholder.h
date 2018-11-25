#ifndef STATEHOLDER_H
#define STATEHOLDER_H

#include "neve.h"

//
// Class for tracking state
//
class StateHolder {
public:
    StateHolder(char const* name, Neve::State state);

    char* toString() const;
    void logState() const;

    StateHolder & operator= (const Neve::State other);

    inline bool operator== (const StateHolder& other)
    {
        return m_state == other.m_state;
    }

    inline bool operator!= (const StateHolder& other)
    {
        return !(*this == other);
    }


    inline bool operator== (const Neve::State state)
    {
        return m_state == state;
    }

    inline bool operator!= (const Neve::State state)
    {
        return !(*this == state);
    }

    inline Neve::State state() { return m_state; }

private:
    Neve::State m_state;
    char const* m_name;
};

#endif // STATEHOLDER_H
