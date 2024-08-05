#global testerror "Err0258"

struct XY
{
}

enum Z: string
{
    z = "Z"
}

enum XYZ: string
{
    using XY
    using Z
}