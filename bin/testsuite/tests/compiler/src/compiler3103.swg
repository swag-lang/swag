interface TOTO
{
    func print(me);
}

struct TITI {}

const COMPILE = false
#if COMPILE
{
    impl TOTO for TITI
    {
        func print(me) {}
    }
}

//#[Swag.PrintBc]
#test
{
    var x: TOTO
    var y: TITI
}
