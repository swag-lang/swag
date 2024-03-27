//#global testerror

struct A {
    x: s32
}

#test
{
    var a: A
    with a
    {
        var x: bool
        //@print(x)
    }
}