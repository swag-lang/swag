#global namespace Swag

struct SystemAllocator
{
}

impl IAllocator for SystemAllocator
{
    func free(request: *AllocatorRequest)
    {
        if !request.address
            return
        ptr := cast(^^void) request.address
        @free(ptr[bitcast(uint) -1'int])
        request.address = null
    }

    mtd req(request: *AllocatorRequest)
    {
        // We are considering that the default system heap alloc does not align
        // So we need to trick it to respect the user wanted alignement.
        // -> store the real system alloc returned pointer before the user (aligned) pointer
        // -> allocate more space to be able to store that pointer, and to respect the alignement
        // So this involves extra space !
        switch request.mode
        {
        case Free:
            free(request)

        case Alloc:
            request.address = null
            fallthrough

        case Realloc:
            // Special case
            if request.size == 0
            {
                if request.mode == .Realloc
                    free(request)
                request.address = null
                return
            }

            size := request.size
            size += @sizeof(*void)
            alignement := request.alignement orelse @sizeof(*void)
            size += alignement - 1

            storePtr := cast(^^void) request.address
            realPtr := cast(^u8) @realloc(storePtr ? storePtr[bitcast(uint) -1'int] : null, size)

            userPtr := realPtr + @sizeof(*void)
            userPtr = cast(^u8) (cast(uint) userPtr & ~(cast(uint) alignement - 1))
            if userPtr != realPtr + @sizeof(*void)
                userPtr += alignement

            storePtr = cast(^^void) userPtr
            storePtr[bitcast(uint) -1'int] = realPtr
            request.address = userPtr
        }
    }
}