#global testerror "Err0743"
struct Point {}

impl Point
{
    func opDrop(me, x: s32) {}
}

//#[Swag.PrintBc]
#test
{
}
