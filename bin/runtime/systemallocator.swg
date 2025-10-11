#global namespace Swag

struct SystemAllocator {}

impl IAllocator for SystemAllocator
{
    func free(request: *AllocatorRequest)
    {
        if !request.address do
            return
        let ptr = cast([*] [*] void) request.address
        @free(ptr[cast #bit (u64) -1's64])
        request.address = null
    }

    mtd impl req(request: *AllocatorRequest)
    {
        // We are considering that the default system heap alloc does not align
        // So we need to trick it to respect the user wanted alignment.
        // -> store the real system alloc returned pointer before the user (aligned) pointer
        // -> allocate more space to be able to store that pointer, and to respect the alignment
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
                if request.mode == .Realloc do
                    free(request)
                request.address = null
                return
            }

            var size = request.size
            size += #sizeof(*void)
            let alignment = request.alignment orelse #sizeof(*void)
            size += alignment - 1

            var storePtr = cast([*] [*] void) request.address
            let realPtr  = cast([*] u8) @realloc(storePtr ? storePtr[cast #bit (u64) -1's64] : null, size)

            var userPtr = realPtr + #sizeof(*void)
            userPtr = cast([*] u8) (cast(u64) userPtr & ~(cast(u64) alignment - 1))
            if userPtr != realPtr + #sizeof(*void) do
                userPtr += alignment

            storePtr                         = cast([*] [*] void) userPtr
            storePtr[cast #bit (u64) -1's64] = realPtr
            request.address                  = userPtr
        }
    }
}
