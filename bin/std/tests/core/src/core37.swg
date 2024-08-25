using Core.Input

#test
{
    var k: Mouse

    k.clear()
    @assert(!k.isButtonPressed(MouseButton.Middle))

    //for
    while false
    {
        k.update()
        var mv = k.getMove()
        if mv.x > 0
        {
            //@print(cast(s64) mv.y)
            //@print("\n")
        }
    }
}
