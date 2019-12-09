#import "std"
using std.input

#test
{
    var k: GamePad
    
    k.clear()
    @assert(!k.isButtonPressed(GamePadButton.X))
    
    //while true
    while false
    {
        k.update()
        if k.isButtonPressed(GamePadButton.X)
        {
            //@print("X\n")
        }
        
        m := k.getMove(GamePadButton.LeftStick)
        if m.x > 0
        {
            //@print(cast(f64) m.x)
            //@print("\n")
        }
    }
}