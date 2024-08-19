#global testerror "Err0716"

enum XY: string
{
    x = "X"
    y = "Y"
}

enum Z: string
{
    z = "Z"
}

enum XYZ: string
{
    using XY1
    using Z
}