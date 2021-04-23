
using core.memory

#test
{
    //#assert reflection.hasDrop(std.file.FileInfo)

    dbg := DebugAllocator.create()

    ptr := alloc(16, dbg)
    ptr1 := new'String(dbg)

    //dbg.printAllocated()

    free(ptr, 16, dbg)
    delete'String(ptr1, dbg)
    @assert(dbg.isEmpty())
}