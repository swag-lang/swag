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
        if k.isButtonPressed(GamePadButton.X)
        {
            //@print("X\n")
        }

        var m = k.getPosition(GamePadButton.LeftTrigger)
        //@print(cast(f64) m.y)
        //@print("\n")
    }
}
