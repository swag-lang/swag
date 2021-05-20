using core.Input

#test
{
    var k: Keyboard
    @assert(!k.isKeyPressed(Key.Space))
    @assert(!k.isKeyJustPressed(Key.Space))
    @assert(k.isKeyReleased(Key.Space))
    @assert(!k.isKeyJustReleased(Key.Space))

    while false
    //loop
    {
        k.update()
        if k.isKeyJustPressed(Key.Space)
        {
            @print("pressed\n")
        }
    }
}