namespace std.memory;
using swag

public struct DebugAllocator
{
    allocator:  IAllocator
    first:      *void
}

// This will be address before and after each allocated block
private
{
    const MagicAlloc = 0x12345678
    const MagicFree  = 0x87654321
    const AllocByte  = 0xAB
    const FreeByte   = 0xFB

    struct Header
    {
        magic:  u32
        size:   u32
        loc:    CompilerSourceLocation
        prev:   *Header
        next:   *Header
    }

    struct Footer
    {
        magic:  u32
    }
}

public impl DebugAllocator
{
    func assertIsAllocated(addr: *void)
    {
        if( !addr) return
        orgAddr := cast(*u8) addr

        ptrHeader := cast(*Header) (orgAddr - @sizeof(Header))
        debug.assert(ptrHeader.magic == MagicAlloc)

        ptrFooter := cast(*Footer) (orgAddr + ptrHeader.size)
        debug.assert(ptrFooter.magic == MagicAlloc)
    }
}

public impl IAllocator for DebugAllocator
{
    func alloc(using self, request: *AllocatorRequest)
    {
        debug.assert(allocator != null, "missing concrete allocator")

        switch request.mode
        {
            case AllocatorMode.Free:
                if !request.address return
                assertIsAllocated(request.address)

                copyReq := :request
                orgAddr := cast(*u8) copyReq.address
                ptrHeader := cast(*Header) (orgAddr - @sizeof(Header))
                ptrFooter := cast(*Footer) (orgAddr + ptrHeader.size)

                // Unlink from allocated list
                debug.assert(ptrHeader.prev || self.first == ptrHeader)
                if !ptrHeader.prev
                    self.first = ptrHeader.next
                else
                {
                    debug.assert(ptrHeader.prev.next == ptrHeader)
                    ptrHeader.prev.next = ptrHeader.next
                }

                if ptrHeader.next
                {
                    debug.assert(ptrHeader.next.prev == ptrHeader)
                    ptrHeader.next.prev = ptrHeader.prev
                }

                // Set block as free
                ptrHeader.magic = MagicFree
                ptrFooter.magic = MagicFree

                // Really free the memory
                copyReq.address = ptrHeader
                allocator.alloc(&copyReq)

                // Fill the memory with garbage
                memory.set(orgAddr, FreeByte, request.size)

            case AllocatorMode.FreeAll:
                allocator.alloc(request)

            case AllocatorMode.Realloc:
                assertIsAllocated(request.address)
                fallthrough

            case AllocatorMode.Alloc:
                copyReq := :request
                copyReq.size += @sizeof(Header) + @sizeof(Footer)
                allocator.alloc(&copyReq)

                orgAddr := cast(*u8) copyReq.address

                header := cast(*Header) copyReq.address
                header.magic = MagicAlloc
                header.size = request.size
                header.loc = copyReq.callerLoc

                footer := cast(*Footer) orgAddr + request.size
                footer.magic = MagicAlloc

                // Link list
                header.prev = null
                header.next = self.first
                if header.next
                    header.next.prev = header
                else
                    self.first = header
                break
        }
    }
}