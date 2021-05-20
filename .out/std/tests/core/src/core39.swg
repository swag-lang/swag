
using Core.Input

#test
{
    var k: GamePad

    k.clear()
    @assert(!k.isButtonPressed(GamePadButton.X))

    //loop
    while false
    {
        assume k.update()
        if k.isButtonJustPressed(GamePadButton.RightThumbstickDown)
            @print("down\n")
        if k.isButtonJustPressed(GamePadButton.RightThumbstickUp)
            @print("up\n")
		if k.isButtonJustPressed(GamePadButton.RightThumbstickLeft)
            @print("left\n")
		if k.isButtonJustPressed(GamePadButton.RightThumbstickRight)
            @print("right\n")
    }
}