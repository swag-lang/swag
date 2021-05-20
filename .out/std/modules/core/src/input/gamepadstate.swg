#global public
#global namespace Input
using math

// Represents specific information about the state of the controller,
// including the current state of buttons and sticks
struct GamePadState
{
    pressed:    [@countof(GamePadButton)] bool
    position:   [@countof(GamePadButton)] Point
}

impl GamePadState
{
    // Reset state to its default value
    func clear(using self)
    {
        Memory.clear(&pressed, cast(uint) @sizeof(pressed))
        Memory.clear(&position, cast(uint) @sizeof(position))
    }
}
