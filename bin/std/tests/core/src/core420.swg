using Core

func toto() throw
{
    defer(err) throw cast(string) Format.toString("titi")
    throw "toto"
}

#test
{
    catch toto()
}