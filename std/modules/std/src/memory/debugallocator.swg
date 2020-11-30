namespace std.memory;
using swag

public struct DebugAllocator
{
    allocator: IAllocator
}

const MagicAlloc = 0x12345678
const MagicFree =  0x87654321

struct Header
{
    loc:    CompilerSourceLocation
    magic:  u32
}

struct Footer
{
    magic:  u32
}

public impl IAllocator for DebugAllocator
{
    func alloc(using self, request: *AllocatorRequest)
    {
        copyReq := :request

        switch request.mode
        {
            case AllocatorMode.Free:
                ptr := request.address

            case AllocatorMode.FreeAll:
                ptr := request.address

            case AllocatorMode.Alloc:
                copyReq.size += @sizeof(Header) + @sizeof(Footer)
                allocator.alloc(&copyReq)
                break

            case AllocatorMode.Realloc:
                break
        }
    }
}