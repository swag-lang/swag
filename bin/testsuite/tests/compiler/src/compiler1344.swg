interface TOTO
{
    func print(me, x: string);
}

struct TITI {}

impl TOTO for TITI
{
    func impl print(me, x: string)
    {
        //@print(x)
    }
}

//#[Swag.PrintBc]
#test
{
    var x: TOTO
    var y: TITI
    x = y
    //y.print("toto\n")
}
