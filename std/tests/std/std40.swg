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
        if k.isButtonJustPressed(GamePadButton.LeftTrigger)
            @print("down\n")
    }
}