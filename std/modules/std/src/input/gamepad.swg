namespace std.input
{
    using std.math

    public struct GamePad
    {
        previousState:  GamePadState
        currentState:   GamePadState
        padIndex:       u32
    }

    impl GamePad
    {
        public
        {
            func update(using self)
            {
                previousState = currentState
                currentState.update(padIndex)
            }

            func clear(using self)
            {
                previousState.clear()
                currentState.clear()
                padIndex = 0
            }

            func isButtonPressed(using self, button: GamePadButton)->bool
            {
                return currentState.isButtonPressed(button)
            }

            func isButtonJustPressed(using self, button: GamePadButton)->bool
            {
                return currentState.isButtonPressed(button) && !previousState.isButtonPressed(button)
            }

            func isButtonReleased(using self, button: GamePadButton)->bool
            {
                return currentState.isButtonReleased(button)
            }

            func isButtonJustReleased(using self, button: GamePadButton)->bool
            {
                return currentState.isButtonReleased(button) && !previousState.isButtonReleased(button)
            }

            func getPosition(using self, button: GamePadButton)->Point
            {
                return currentState.getPosition(button)
            }
        }
    }
}
