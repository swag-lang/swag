//#global testerror "Err0706"

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

const C = XYZ.XY.X
const D = XYZ.XY.Y
const E = XYZ.Z.Z
#assert XYZ.XY.X == "1"
#assert XYZ.XY.Y == "2"
#assert XYZ.Z.Z  == "Z"