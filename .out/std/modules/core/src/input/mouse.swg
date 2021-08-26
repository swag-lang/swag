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
    method update()
    {
        previousState = currentState
        currentState.update()
    }

    // Reset the mouse state to its default value
    method clear()
    {
        previousState.clear()
        currentState.clear()
    }

    // Determines whether the specified mouse button is pressed
    method isButtonPressed(button: MouseButton)->bool
    {
        return currentState.pressed[button]
    }

    // Determines whether the specified mouse button has just been pressed
    method isButtonJustPressed(button: MouseButton)->bool
    {
        return currentState.pressed[button] and !previousState.pressed[button]
    }

    // Determines whether the specified mouse button is released
    method isButtonReleased( button: MouseButton)->bool
    {
        return !currentState.pressed[button]
    }

    // Determines whether the specified mouse button has just been released
    method isButtonJustReleased(button: MouseButton)->bool
    {
        return !currentState.pressed[button] and previousState.pressed[button]
    }

    // Returns the previous mouse position, before the last update
    method getPreviousPos()->Point
    {
        return previousState.position
    }

    // Returns the mouse position
    method getPosition()->Point
    {
        return currentState.position
    }

    // Returns the move between the current position and the previous one
    method getMove()->Point
    {
        return currentState.position - previousState.position
    }
}
