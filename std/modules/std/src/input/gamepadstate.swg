namespace std.input
{
    using std.math
    public struct GamePadState
    {
        pressed:    [@countof(GamePadButton)] bool
        movement:   [@countof(GamePadButton)] Point
    }

    impl GamePadState
    {
        public
        {
            func update(using self, padIndex: u32)
            {
                std.input.os.getGamePadState(padIndex, self)
            }

            func clear(using self)
            {
                memory.clear(&pressed, @sizeof(pressed))
                memory.clear(&movement, @sizeof(movement))
            }

            func isButtonPressed(using self, button: GamePadButton)->bool
            {
                return pressed[cast(u32) button]
            }

            func isButtonReleased(using self, button: GamePadButton)->bool
            {
                return !pressed[cast(u32) button]
            }

            func getMove(using self, button: GamePadButton)->Point
            {
                return movement[cast(u32) button]
            }
        }
    }
}
