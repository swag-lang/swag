#global testerror
using Core

func toto()->Array'string
{
    var result: retval
    result.add("toto")
    return result
}

#test
{
    c := #run toto()
}