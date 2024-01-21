#global testerror "Err0262"

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