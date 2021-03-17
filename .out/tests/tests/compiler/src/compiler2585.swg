func toto() throw
{
    throw "toto"
}

//#[swag.printbc]
#test
{
    var x: func() throw
    x = &toto
    catch x()
    @assert(@err == "toto")
}