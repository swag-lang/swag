#test
{
    using Core.Input
    while !Keyboard.isKeyPressed(Key.Space)
    {
        var pt = getMousePos()
        //@print(cast(s64) pt.item0)
        //@print("\n")
        break
    }

    //@print("pressed")
    while Keyboard.isKeyPressed(Key.Space):
        break

    //@print("unpressed")
}
