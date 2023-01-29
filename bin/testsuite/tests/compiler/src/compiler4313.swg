//#global testerror
#global skip
#test
{
    var x: func(x: s32, y: s32 = 1)

    x = func(a, b) {
    }

    x()
}
