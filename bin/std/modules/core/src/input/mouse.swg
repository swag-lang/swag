#global public
#global namespace Input
using Math

// Allows reading position and button click information from mouse
struct Mouse
{
    previousState:	MouseState
    currentState:	MouseState
}

impl Mouse
{
    // Compute the current state of the mouse
    mtd update()
    {
        previousState = currentState
        currentState.update()
    }

    // Reset the mouse state to its default value
    mtd clear()
    {
        previousState.clear()
        currentState.clear()
    }

    // Determines whether the specified mouse button is pressed
    mtd isButtonPressed(button: MouseButton)->bool
    {
        return currentState.pressed[button]
    }

    // Determines whether the specified mouse button has just been pressed
    mtd isButtonJustPressed(button: MouseButton)->bool
    {
        return currentState.pressed[button] and !previousState.pressed[button]
    }

    // Determines whether the specified mouse button is released
    mtd isButtonReleased( button: MouseButton)->bool
    {
        return !currentState.pressed[button]
    }

    // Determines whether the specified mouse button has just been released
    mtd isButtonJustReleased(button: MouseButton)->bool
    {
        return !currentState.pressed[button] and previousState.pressed[button]
    }

    // Returns the previous mouse position, before the last update
    mtd getPreviousPos()->Point
    {
        return previousState.position
    }

    // Returns the mouse position
    mtd getPosition()->Point
    {
        return currentState.position
    }

    // Returns the move between the current position and the previous one
    mtd getMove()->Point
    {
        return currentState.position - previousState.position
    }
}
