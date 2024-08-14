#global testerror "Err0698"

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

const C = XYZ.X1