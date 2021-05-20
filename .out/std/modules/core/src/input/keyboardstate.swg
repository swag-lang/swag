#global namespace Input

// Represents one keyboard state
public struct KeyboardState
{
    // Whether each key is pressed or not
    pressed: [@countof(Key)] bool
}

public impl KeyboardState
{
    // Compute current keyboard state
    func update(using self)
    {
        loop i: @countof(Key)
        {
            pressed[i] = isKeyPressed(cast(Key) i)
        }
    }

    // Reset keyboard state
    func clear(using self)
    {
        Memory.clear(&pressed, cast(uint) @sizeof(pressed))
    }
}
