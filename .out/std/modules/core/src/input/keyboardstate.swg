#global public
#global namespace Input

// Represents one keyboard state
struct KeyboardState
{
    pressed: [@countof(Key)] bool
}

impl KeyboardState
{
    // Compute current keyboard state
    method update()
    {
        loop i: @countof(Key)
        {
            pressed[i] = isKeyPressed(cast(Key) i)
        }
    }

    // Reset keyboard state
    method clear()
    {
        Memory.clear(&pressed, cast(uint) @sizeof(pressed))
    }
}
