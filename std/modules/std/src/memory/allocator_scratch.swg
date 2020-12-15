namespace memory;
using swag

public struct ScratchAllocator
{
    fallback:   IAllocator
    block:      *void
    used:       uint
    leaks:      Array'{ptr: *void, size: uint}
}

public impl IAllocator for ScratchAllocator
{
    func alloc(using self, request: *AllocatorRequest)
    {
        switch request.mode
        {
            case AllocatorMode.Free:
                break

            case AllocatorMode.FreeAll:
                used = 0
                visit *p: leaks
                {
                    var newReq: AllocatorRequest
                    newReq.mode = AllocatorMode.Free
                    newReq.address  = p.ptr
                    newReq.size     = p.size
                    fallback.alloc(&newReq)
                }
                break

            case AllocatorMode.Alloc:
                break
        }
    }
}