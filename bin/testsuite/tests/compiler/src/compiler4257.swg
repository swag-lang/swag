#global testerror
#test
{
    x := "toto"
    y := x[..<0]
    @print(y)
}
