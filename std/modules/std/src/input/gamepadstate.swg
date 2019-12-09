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
        }
    }
}
