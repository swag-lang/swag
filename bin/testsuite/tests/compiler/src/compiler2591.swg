#global testerror "Err0747"

using Swag

#[Swag.Overload]
func alloc(size: u64, allocator: IAllocator = null, alignement: u32 = 0, hint: string = null, loc = #callerlocation)->*void
{
    return null
}

#[Swag.Overload]
func alloc(size: bool, allocator: IAllocator = null, alignement: u32 = 0, hint: string = null, loc = #callerlocation)->*void
{
    return null
}

func talloc(size: u64, alignement: u32 = 0, hint: string = null, loc = #callerlocation)->*void
{
    let cxt = @getcontext()
    cxt.tempAllocator.alloc(10)
    return null
}
