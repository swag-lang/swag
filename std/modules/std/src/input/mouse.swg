namespace std.input
{
	using std.math

    // Allows reading position and button click information from mouse
	public struct Mouse
	{
		previousState:	MouseState
		currentState:	MouseState
	}

	public impl Mouse
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
        func isButtonPressed(using self, button: MouseButton)->bool
        {
            return currentState.pressed[cast(u32) button]
        }

        // Determines whether the specified mouse button has just been pressed
        func isButtonJustPressed(using self, button: MouseButton)->bool
        {
            return currentState.pressed[cast(u32) button] && !previousState.pressed[cast(u32) button]
        }

        // Determines whether the specified mouse button is released
        func isButtonReleased(using self, button: MouseButton)->bool
        {
            return !currentState.pressed[cast(u32) button]
        }

        // Determines whether the specified mouse button has just been released
        func isButtonJustReleased(using self, button: MouseButton)->bool
        {
            return !currentState.pressed[cast(u32) button] && previousState.pressed[cast(u32) button]
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
}
