public namespace mouse;
using math

// Allows reading position and button click information from mouse
struct Mouse
{
    previousState:	State
    currentState:	State
}

impl Mouse
{
    // Compute the current state of the mouse
    func update(using self)
    {
        previousState = currentState
        currentState.update()
    }

    // Reset the mouse state to its default value
    func clear(using self)
    {
        previousState.clear()
        currentState.clear()
    }

    // Determines whether the specified mouse button is pressed
    func isButtonPressed(using self, button: Button)->bool
    {
        return currentState.pressed[button]
    }

    // Determines whether the specified mouse button has just been pressed
    func isButtonJustPressed(using self, button: Button)->bool
    {
        return currentState.pressed[button] && !previousState.pressed[button]
    }

    // Determines whether the specified mouse button is released
    func isButtonReleased(using self, button: Button)->bool
    {
        return !currentState.pressed[button]
    }

    // Determines whether the specified mouse button has just been released
    func isButtonJustReleased(using self, button: Button)->bool
    {
        return !currentState.pressed[button] && previousState.pressed[button]
    }

    // Returns the previous mouse position, before the last update
    func getPreviousPos(using self)->Point
    {
        return previousState.position
    }

    // Returns the mouse position
    func getPosition(using self)->Point
    {
        return currentState.position
    }

    // Returns the mouse move, between the current position and the previous one
    func getMove(using self)->Point
    {
        return currentState.position - previousState.position
    }
}
