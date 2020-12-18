#import "core"
using core.gamepad

#test
{
    var k: GamePad

    k.clear()
    @assert(!k.isButtonPressed(Button.X))

    //while true
    while false
    {
        k.update()
        if k.isButtonJustPressed(Button.LeftTrigger)
            @print("down\n")
    }
}