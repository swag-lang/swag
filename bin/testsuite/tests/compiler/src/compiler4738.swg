#global testerror "Err0484"
func toto(x: s32, y: f32) {}

struct A
{
    c: func(x: s32, y: f32)
}

#test
{
    var a: A
    a.c = &toto

    a.c(1's32)
}