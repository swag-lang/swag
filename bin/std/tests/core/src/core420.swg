using Core

func toto() throw
{
    defer(err) throw Format.toString("titi")
    throw "toto"
}

#test
{
    catch toto()
}