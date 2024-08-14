#global testerror "Err0604"

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