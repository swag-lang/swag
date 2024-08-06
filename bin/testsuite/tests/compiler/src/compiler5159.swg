#global testerror "Err0252"

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