#global namespace swag

struct SystemAllocator
{
}

impl IAllocator for SystemAllocator
{
    func alloc(self, request: *AllocatorRequest)
    {
        // We are considering that the default system heap alloc does not align
        // So we need to trick it to respect the user wanted alignement.
        // -> store the real system alloc returned pointer before the user (aligned) pointer
        // -> allocate more space to be able to store that pointer, and to respect the alignement
        // So this involves extra space !
        switch request.mode
        {
            case AllocatorMode.Free:
                if !request.address break
                ptr := cast(**void) request.address
                @free(ptr[-1])
                request.address = null

            case AllocatorMode.Alloc:
                request.address = null
                fallthrough

            case AllocatorMode.Realloc:
                size := request.size
                size += @sizeof(*void)
                alignement := request.alignement ?? @sizeof(*void)
                size += alignement - 1

                storePtr := cast(**void) request.address
                realPtr := cast(*u8) @realloc(storePtr ? storePtr[-1] : null, size)

                userPtr := cast(*u8) (cast(uint) realPtr & ~(cast(uint) alignement - 1))
                if userPtr != realPtr userPtr += alignement
                userPtr += @sizeof(*void)

                storePtr = cast(**void) userPtr
                storePtr[-1] = realPtr
                request.address = userPtr
        }
    }
}