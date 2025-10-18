#global testerror "Err0746"

struct Point { x, y: s32 }

//#[Swag.PrintBc]
#test
{
    var pt: Point
    var ptr = &pt
    @init(ptr, 1)(10, true)
}
