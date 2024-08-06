#global testerror "Err0707"
struct A
{
    x: s32
}
struct B
{
    a: *A
}

enum RGB { R; }
func toto(rgb: RGB) {}

#test
{
    var a: A
    var b: B

    with b.a = &a
    {
        toto(.R1)
    }
}