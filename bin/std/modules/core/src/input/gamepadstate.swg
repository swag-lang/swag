#global public
#global namespace Input
using Math

// Represents specific information about the state of the controller,
// including the current state of buttons and sticks
struct GamePadState
{
    pressed:      [@countof(GamePadButton)] bool
    position:     [@countof(GamePadButton)] Point
}

impl GamePadState
{
    // Reset state to its default value
    mtd clear()
    {
        Memory.clear(&.pressed, #sizeof(.pressed))
        Memory.clear(&.position, #sizeof(.position))
    }
}
