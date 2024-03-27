
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
        if k.isButtonJustPressed(GamePadButton.RightThumbstickDown) do
            @print("down\n")
        if k.isButtonJustPressed(GamePadButton.RightThumbstickUp) do
            @print("up\n")
		if k.isButtonJustPressed(GamePadButton.RightThumbstickLeft) do
            @print("left\n")
		if k.isButtonJustPressed(GamePadButton.RightThumbstickRight) do
            @print("right\n")
    }
}