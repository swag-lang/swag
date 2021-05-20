
using core.GamePad

#test
{
    var k: Pad

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