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
			/// Reset state to its default value
            func clear(using self)
            {
                memory.clear(&pressed, @sizeof(pressed))
                memory.clear(&position, @sizeof(position))
            }
        }
    }
}
