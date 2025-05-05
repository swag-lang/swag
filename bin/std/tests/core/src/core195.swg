using Core.Memory

#test
{
    //#assert(Reflection.hasDrop(std.File.FileInfo))

    var dbg: Swag.DebugAllocator
    dbg.setup(@sysalloc())

    let ptr  = alloc(16, dbg)
    let ptr1 = new'String(dbg)

    //dbg.printAllocated()

    free(ptr, 16, dbg)
    delete'String(ptr1, dbg)
    @assert(dbg.countAlloc == 0)
}
