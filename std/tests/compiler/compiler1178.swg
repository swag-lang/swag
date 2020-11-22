using swag
enum TOTO : s64
{
    A = 666
    B = 100
}

#test
{
    x := TOTO.A
    y := TOTO.B
    ptr := &x
    y = autocast ptr[0]
}