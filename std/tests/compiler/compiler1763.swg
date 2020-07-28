struct S
{
    x, y: f64 = 50
}

enum TITI
{
    AA
    BB
}

//#[swag.printbc]
#test
{
    func toto()
    {
        @print("xxxx")
    }

    var pt: S
    pt.x = 41
    toto()

    x := TITI.AA
    x = TITI.BB
}