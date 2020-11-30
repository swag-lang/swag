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
        magic:      u32
        size:       u32
        userAddr:   *void
        loc:        CompilerSourceLocation
        prev:       *Header
        next:       *Header
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
                copyReq.address = ptrHeader.userAddr
                allocator.alloc(&copyReq)

                // Fill the memory with garbage
                memory.set(orgAddr, FreeByte, request.size)

            case AllocatorMode.FreeAll:
                allocator.alloc(request)
                self.first = null

            case AllocatorMode.Realloc:
                assertIsAllocated(request.address)
                fallthrough

            case AllocatorMode.Alloc:
                copyReq := :request

                // Adapt size to be able to store additional informations
                // Take care of alignement request, because we need to be sure
                // that the returned memory block is well aligned
                alignement := request.alignement ?? @sizeof(*void)
                toAdd := @sizeof(Header) + @sizeof(Footer)
                addAlign := toAdd & ~(alignement - 1)
                if addAlign < toAdd
                    addAlign += alignement
                copyReq.size += addAlign
                allocator.alloc(&copyReq)

                orgAddr := cast(*u8) copyReq.address
                orgAddr += addAlign
                request.address = orgAddr // This is the aligned returned user address

                header := cast(*Header) (orgAddr - @sizeof(Header))
                header.magic = MagicAlloc
                header.userAddr = copyReq.address
                header.size = request.size
                header.loc = copyReq.callerLoc

                footer := cast(*Footer) (orgAddr + request.size)
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