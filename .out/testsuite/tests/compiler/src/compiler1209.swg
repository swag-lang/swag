
#global testerror

public
#[Swag.inline]
{
    func toto()
    {
        toto1()
    }

    func toto1()
    {
        //@print("toto\n")
    }
}

//#[Swag.PrintBc]
#test
{
    toto()
}