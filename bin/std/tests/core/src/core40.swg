using Core.Input

#test
{
    var k: GamePad

    k.clear()
    @assert(!k.isButtonPressed(GamePadButton.X))

    //for
    while false
    {
        assume k.update()
        if k.isButtonJustPressed(GamePadButton.LeftTrigger):
            @print("down\n")
    }
}
