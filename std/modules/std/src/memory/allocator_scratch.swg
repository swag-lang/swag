namespace memory;
using swag

public struct ScratchAllocator
{
    fallback:   IAllocator

    block:  *void
    used:   uint
    //leak:   Array'{*void, uint}
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
                break

            case AllocatorMode.Alloc:
                break
        }
    }
}