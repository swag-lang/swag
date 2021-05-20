
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
        if k.isButtonJustPressed(Button.RightThumbstickDown)
            @print("down\n")
        if k.isButtonJustPressed(Button.RightThumbstickUp)
            @print("up\n")
		if k.isButtonJustPressed(Button.RightThumbstickLeft)
            @print("left\n")
		if k.isButtonJustPressed(Button.RightThumbstickRight)
            @print("right\n")
    }
}