func toto() throw
{
    throw "toto"
}

func titi() throw
{
    throw "titi"
}

#test
{
    catch
    {
        toto()
        titi()
    }

    @assert(@errmsg == "titi")
}