func toto() throw
{
    throw "toto"
}

//#[Swag.PrintBc]
#test
{
    var x: func() throw
    x = &toto
    catch x()
    @assert(@err == "toto")
}