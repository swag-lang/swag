
#global testerror

interface TOTO
{
	print: func()
}

struct TITI
{
}

const COMPILE = false
#if COMPILE
{
    impl TOTO for TITI
    {
        func print(using self)
        {
        }
    }
}

//#[Swag.PrintBc]
#test
{
    var x: TOTO
    var y: TITI
    x = y
}