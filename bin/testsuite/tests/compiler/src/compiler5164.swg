//#global testerror "Err0649"

enum XY : string
{
    X = "1"
    Y = "2"
}

enum Z : string
{
    Z = "Z"
}

enum XYZ : string
{
    using XY
    using Z
}

func toto(x: XY)
{
}

#test
{
    toto(XYZ.XY.X)
}
