#global testerror "Err0588"

enum XY: s32
{
    x = 1
    y = 2
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