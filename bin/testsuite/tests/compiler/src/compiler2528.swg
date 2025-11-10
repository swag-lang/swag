struct ErrToto {}
func toto()->s64 throw
{
    throw ErrToto{}
    return 555
}

//#[Swag.PrintBc]
#test
{
    var x = catch toto()
}
