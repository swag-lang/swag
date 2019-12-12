namespace std.input
{
    public struct KeyboardState
    {
        pressed: [@countof(Key)] bool
    }

    impl KeyboardState
    {
        public
        {
            func update(using self)
            {
                loop i: @countof(Key)
                {
                    pressed[i] = isKeyPressed(cast(Key) i)
                }
            }

            func clear(using self)
            {
                memory.clear(&pressed, @sizeof(pressed))
            }
        }
    }
}
