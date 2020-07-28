struct S
{
    x, y: f64 = 50
}

enum TITI
{
    AA
    BB
}

    func toto(xx: s32, yy: [..] s32)
    {
        @print(cast(s64)yy[0])
    }

//#[swag.printbc]
#test
{
    x := [10, 20, 30]
    toto(666, x)
}