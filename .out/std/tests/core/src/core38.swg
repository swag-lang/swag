
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
        if k.isButtonPressed(Button.X)
        {
            //@print("X\n")
        }

        m := k.getPosition(Button.LeftTrigger)
        //@print(cast(f64) m.y)
        //@print("\n")
    }
}