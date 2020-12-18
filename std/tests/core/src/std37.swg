#import "core"
using core.mouse

#test
{
    var k: Mouse

    k.clear()
    @assert(!k.isButtonPressed(Button.Middle))

    //while true
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