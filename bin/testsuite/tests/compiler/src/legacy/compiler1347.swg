interface TOTO
{
    func print(v: *TOTO, x: bool);
}

struct TITI {}

impl TOTO for TITI
{
    //#[Swag.PrintBc]
    func impl print(me, x: bool)
    {
        //@print("x")
    }
}

//#[Swag.PrintBc]
#test
{
    var x: TOTO
    var y: TITI
    x = y
    //x.print(true)
}
