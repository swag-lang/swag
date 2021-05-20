using Core.Input

#test
{
    var k: Mouse

    k.clear()
    @assert(!k.isButtonPressed(MouseButton.Middle))

    //loop
    while false
    {
        k.update()
        mv := k.getMove()
        if mv.x > 0
        {
            //@print(cast(s64) mv.y)
            //@print("\n")
        }
    }
}