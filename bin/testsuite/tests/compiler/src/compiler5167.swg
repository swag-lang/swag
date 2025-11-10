//#global skip

enum XY: string
{
    X = "1"
    Y = "2"
}

enum Z: string
{
    Z = "Z"
}

enum XYZ: string { using XY, using Z }

func toto(x: XYZ)
{
    @assert(x == XYZ.XY.X)
}

#test
{
    let x: XYZ = XY.X
    toto(x)
}
