
using core.Input

#test
{
    var k: GamePad

    k.clear()
    @assert(!k.isButtonPressed(GamePadButton.X))

    //loop
    while false
    {
        assume k.update()
        if k.isButtonJustPressed(GamePadButton.LeftTrigger)
            @print("down\n")
    }
}