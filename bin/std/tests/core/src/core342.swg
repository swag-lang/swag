#global skip
using Core

var G: s32

#test
{
    var x: Array's32
    x.add(1)
    x.add(2)
    x.add(3)

    var ptr: closure(s32)
    visit v: x
    {
        ptr = @|v|(value)
        {
            G += value
        }
        ptr(0)
    }
}