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
        
        m := k.getPosition(GamePadButton.LeftTrigger)
        //@print(cast(f64) m.y)
        //@print("\n")
    }
}