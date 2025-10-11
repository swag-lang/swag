using Swag
enum TOTO: s64
{
    A = 666
    B = 100
}

#test
{
    var x   = TOTO.A
    var y   = TOTO.B
    let ptr = &x
    y = cast() dref ptr
}
