#global testerror "Err0258"
struct A
{
    x: s32
}

func toto()
{
    var a: A
                                                               var c = &a[1]
    @print(c.x)
}