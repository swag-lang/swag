namespace memory;
using swag

public struct ScratchAllocator
{
    fallback:   IAllocator

    block:  *void
    used:   u32
}

public impl IAllocator for ScratchAllocator
{
    func alloc(using self, request: *AllocatorRequest)
    {
        switch request.mode
        {
            case AllocatorMode.Alloc:
                break
        }
    }
}