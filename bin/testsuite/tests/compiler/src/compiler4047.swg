#global testerror
interface I
{
    mtd toto()
}
#test
{
    var a: I
    b := 2
    c := a == b
}
