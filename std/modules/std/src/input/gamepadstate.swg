namespace std.input
{
    using std.math
    
    /// Represents specific information about the state of the controller, 
    /// including the current state of buttons and sticks
    public struct GamePadState
    {
        pressed:    [@countof(GamePadButton)] bool
        position:   [@countof(GamePadButton)] Point
    }

    impl GamePadState
    {
        public
        {
            func clear(using self)
            {
                memory.clear(&pressed, @sizeof(pressed))
                memory.clear(&position, @sizeof(position))
            }

            /// Determines whether specified input device buttons are pressed in this GamePadState
            func isButtonPressed(using self, button: GamePadButton)->bool
            {
                return pressed[cast(u32) button]
            }

            /// Determines whether specified input device buttons are released (not pressed) in this GamePadState
            func isButtonReleased(using self, button: GamePadButton)->bool
            {
                return !pressed[cast(u32) button]
            }

            /// Get the analog position of the given button
            func getPosition(using self, button: GamePadButton)->Point
            {
                return position[cast(u32) button]
            }
        }
    }
}
