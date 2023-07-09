#global testerror
using Core

func toto()
{
    var x = Format.toString("%", 1, 2)
}

#test
{
    toto()
}