func toto()->s64 throw
{
    throw "toto"
    return 555
}

//#[Swag.PrintBc]
#test
{
    var g = 0
    var x = catch toto()
    @assert(@errmsg != null)
}