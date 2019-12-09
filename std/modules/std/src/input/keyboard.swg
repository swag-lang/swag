namespace std.input
{
    public struct Keyboard
    {
        internal previousState:  KeyboardState
        internal currentState:   KeyboardState
        internal pressedRepeat:  [@countof(Key)] bool
        internal pressedTime:    [@countof(Key)] u32
        
        pressedRepeatStartTimeMs:   u32 = 400
        pressedRepeatTimeMs:        u32 = 150
        canRepeat:                  bool = true        
    }
    
    impl Keyboard
    {
        public 
        {
            func update(using self)
            {
                previousState = currentState
                currentState.update()
                
                // Repetition
                now := time.nowMilliseconds()
                loop i: @countof(Key)
                {
                    if !isKeyPressed(cast(Key) i)
                    {
                        pressedRepeat[i] = false
                        pressedTime[i] = 0
                    }
                    else if(canRepeat)
                    {
                        // First pressed time
                        if !pressedTime[i]
                            pressedTime[i] = now
                            
                        // First repetition
                        if !pressedRepeat[i]
                        {
                            if (now - pressedTime[i] >= pressedRepeatStartTimeMs)
                            {
                                pressedRepeat[i] = true
                                pressedTime[i]   = now
                                previousState.pressed[i] = false
                            }
                        }
                        else if now - pressedTime[i] >= pressedRepeatTimeMs
                        {
                            pressedTime[i] = now
                            previousState.pressed[i] = false
                        }
                    }
                }
            } 

            func clear(using self)
            {
                previousState.clear()
                currentState.clear()
                @memset(&pressedRepeat, 0, @sizeof(pressedRepeat))
                @memset(&pressedTime, 0, @sizeof(pressedTime))
            }
            
            func isKeyPressed(using self, key: Key)->bool
            {
                return currentState.isKeyPressed(key)
            }
            
            func isKeyJustPressed(using self, key: Key)->bool
            {                
                return currentState.isKeyPressed(key) && !previousState.isKeyPressed(key)
            }            
            
            func isKeyReleased(using self, key: Key)->bool
            {
                return currentState.isKeyReleased(key)
            }            
            
            func isKeyJustReleased(using self, key: Key)->bool
            {
                return currentState.isKeyReleased(key) && !previousState.isKeyReleased(key)
            }              
        }        
    }
}
