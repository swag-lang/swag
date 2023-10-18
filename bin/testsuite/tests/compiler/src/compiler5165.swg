#global skip

enum XY: string
{
    X = "1"
    Y = "2"
}

enum Z: string
{
    Z = "Z"
}

enum XYZ: string
{
    using XY
    using Z
}

func toto(x: XYZ)
{
}

#test
{
    let x = XY.X
    toto(x)
}