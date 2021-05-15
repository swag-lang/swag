
using core.gamepad

#test
{
    var k: GamePad

    k.clear()
    @assert(!k.isButtonPressed(Button.X))

    //loop
    while false
    {
        assume k.update()
        if k.isButtonJustPressed(Button.LeftTrigger)
            @print("down\n")
    }
}