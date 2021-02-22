using swag

func alloc(size: uint, allocator: IAllocator = null, alignement: u32 = 0, hint: string = null, loc = #callerlocation)->*void
{
    return null
}

func talloc(size: uint, alignement: u32 = 0, hint: string = null, loc = #callerlocation)->*void
{
    cxt := @getcontext()
    //cxt.tempAllocator.alloc(10)
    return null
}
