func toto() throw
{
    throw "toto"
}

func titi() throw
{
    throw "titi"
}

//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
#test
{
    catch
    {
        toto()
        titi()
    }

    @assert(cast(string) @err() == "titi")
}