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
                loop @countof(Key)
                    pressed[index] = isKeyPressed(cast(Key) index)
            }
            
            func isKeyPressed(using self, key: Key)->bool
            {
                return pressed[cast(u32) key]
            }
            
            func isKeyReleased(using self, key: Key)->bool
            {
                return !pressed[cast(u32) key]
            }   

            func clear(using self)
            {
                @memset(&pressed, 0, @sizeof(pressed))
            }
        }        
    }
}
