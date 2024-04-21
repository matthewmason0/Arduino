#ifndef ENGINESTATE_H
#define ENGINESTATE_H

class EngineStote
{
public:
    enum State : uint8_t
    {
        OFF = 0,
        STARTING = 1,
        RUNNING = 2
    };

    constexpr EngineStote(State s) : state(s) {}

    constexpr operator State() const { return state; }
    // Prevent usage: if (state)
    explicit operator bool() const = delete;

    

private:
    State state;
};

#endif // ENGINESTATE_H