#global testerror
interface I
{
    mtd toto()
}
#test
{
    var a: I
    var b = 2
    var c = a == b
}
