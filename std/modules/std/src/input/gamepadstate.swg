public namespace gamepad;
using math

// Represents specific information about the state of the controller,
// including the current state of buttons and sticks
struct State
{
    pressed:    [@countof(Button)] bool
    position:   [@countof(Button)] Point
}

impl State
{
    // Reset state to its default value
    func clear(using self)
    {
        memory.clear(&pressed, cast(uint) @sizeof(pressed))
        memory.clear(&position, cast(uint) @sizeof(position))
    }
}
