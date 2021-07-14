#global testerror

using Swag

var G = 0

#[mixin]
func toto()
{
    func titi()
    {
        G += 1
    }

    titi()
}

//#[Swag.PrintBc]
#test
{
    toto()
    toto()
}