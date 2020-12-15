namespace memory;
using swag;

public struct ScratchAllocator
{
    fallback:   IAllocator
    block:      *u8
    used:       uint
    capacity:   uint
    leaks:      Array'(*void)
}

public impl IAllocator for ScratchAllocator
{
    func alloc(using self, request: *AllocatorRequest)
    {
        debug.assert(fallback != null, "missing fallback allocator")
        debug.assert(block != null, "ScratchAllocator should be initialized first")

        switch request.mode
        {
            case AllocatorMode.Free:
                break

            case AllocatorMode.FreeAll:
                used = 0
                visit p: leaks
                {
                    var newReq: AllocatorRequest
                    newReq.mode = AllocatorMode.Free
                    newReq.address  = p
                    fallback.alloc(&newReq)
                }

                leaks.clear()
                break

            case AllocatorMode.Alloc:
                request.address = null
                fallthrough

            case AllocatorMode.Realloc:
                oldAddress := request.address

                usedAlign := memory.align(used, request.alignement)
                if usedAlign + request.size <= capacity
                {
                    request.address = block + usedAlign
                    used = usedAlign + request.size
                }

                // Not enough room. Use fallback allocator
                else
                {
                    fallback.alloc(request)
                    leaks.add(request.address)
                }

                // Move block
                if oldAddress
                    memory.copy(request.address, oldAddress, request.oldSize)

                break
        }
    }
}