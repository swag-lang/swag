namespace std.input
{
	using std.math

	public struct Mouse
	{
		previousState:	MouseState
		currentState:	MouseState
	}

	impl Mouse
	{
		public
		{
			func update(using self)
			{
				previousState = currentState
				currentState.update()
			}

			func clear(using self)
			{
				previousState.clear()
				currentState.clear()
			}

			func isButtonPressed(using self, button: MouseButton)->bool
			{
				return currentState.isButtonPressed(button)
			}

			func isButtonJustPressed(using self, button: MouseButton)->bool
			{
				return currentState.isButtonPressed(button) && !previousState.isButtonPressed(button)
			}

			func isButtonReleased(using self, button: MouseButton)->bool
			{
				return currentState.isButtonReleased(button)
			}

			func isButtonJustReleased(using self, button: MouseButton)->bool
			{
				return currentState.isButtonReleased(button) && !previousState.isButtonReleased(button)
			}

			func getPreviousPos(using self)->Point
			{
				return previousState.position
			}

			func getPos(using self)->Point
			{
				return currentState.position
			}

			func getMove(using self)->Point
			{
				return currentState.position - previousState.position
			}
		}	
	}
}
