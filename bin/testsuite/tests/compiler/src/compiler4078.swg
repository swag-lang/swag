#test
{
    dbg := Swag.DebugAllocator.create()
    defer dbg.printLeaks()
}