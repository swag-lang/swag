#global testerror "Err0202"

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