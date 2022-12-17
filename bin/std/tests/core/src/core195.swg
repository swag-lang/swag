using Core.Memory

#test
{
    //#assert Reflection.hasDrop(std.File.FileInfo)

    var dbg: Swag.DebugAllocator
    dbg.setup(Swag.systemAllocator())

    ptr := alloc(16, dbg)
    ptr1 := new'String(dbg)

    //dbg.printAllocated()

    free(ptr, 16, dbg)
    delete'String(ptr1, dbg)
    @assert(dbg.isEmpty())
}