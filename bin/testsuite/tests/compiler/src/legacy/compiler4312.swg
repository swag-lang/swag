#global testerror "Err0388"
#test
{
    var x: func(x: s32, y: s32)

    x = func(a, b = 1) {}

    x()
}
