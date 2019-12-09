#import "std"
using std.input

#test
{
    var k: Mouse
    
    k.clear()
    @assert(!k.isButtonPressed(MouseButton.Middle))
    
    //while true
    while false
    {
        k.update()
        mv := k.getMove()
        if mv.x > 0
        {
            //@print(cast(s64) mv.x)
            //@print("\n")
        }
    }
}