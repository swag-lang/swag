
#global testerror "Err0367"

public
#[Swag.Inline]
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