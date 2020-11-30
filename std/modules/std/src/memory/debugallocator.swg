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
        userSize:   u32
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

        ptrFooter := cast(*Footer) (orgAddr + ptrHeader.userSize)
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

                orgAddr := cast(*u8) request.address
                ptrHeader := cast(*Header) (orgAddr - @sizeof(Header))
                ptrFooter := cast(*Footer) (orgAddr + ptrHeader.userSize)

                // Unlink from allocated list
                if !ptrHeader.prev
                {
                    debug.assert(self.first == ptrHeader)
                    self.first = ptrHeader.next
                }
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
                copyReq := :request
                copyReq.address = ptrHeader.userAddr
                allocator.alloc(&copyReq)

                // Fill the memory with garbage
                memory.set(orgAddr, FreeByte, ptrHeader.userSize)

            case AllocatorMode.FreeAll:
                allocator.alloc(request)
                self.first = null

            case AllocatorMode.Realloc:
                assertIsAllocated(request.address)
                fallthrough

            case AllocatorMode.Alloc:
                // Adapt size to be able to store additional informations
                // Take care of alignement request, because we need to be sure
                // that the returned memory block is well aligned
                alignement := request.alignement ?? @sizeof(*void)
                toAdd := @sizeof(Header) + @sizeof(Footer)
                toAddAlign := toAdd & ~(alignement - 1)
                if toAddAlign < toAdd toAddAlign += alignement

                // In case of a reallocation, get the previous allocated user size
                // This will be used to fill with garbage the difference
                prevSize := 0'u32
                if request.mode == AllocatorMode.Realloc && request.address
                {
                    prevHeader := cast(*Header) request.address
                    prevSize = prevHeader[-1].userSize
                }

                // Make the real allocation
                copyReq := :request
                copyReq.size += toAddAlign
                allocator.alloc(&copyReq)

                // This is the aligned returned user address
                orgAddr := cast(*u8) copyReq.address
                orgAddr += toAddAlign
                userAddr := orgAddr

                // Setup header
                header := cast(*Header) (orgAddr - @sizeof(Header))
                header.magic = MagicAlloc
                header.userAddr = copyReq.address
                header.userSize = request.size
                header.loc = copyReq.callerLoc

                // Setup footer
                footer := cast(*Footer) (orgAddr + request.size)
                footer.magic = MagicAlloc

                // Fill with garbage
                if request.mode == AllocatorMode.Alloc || !request.address
                    memory.set(userAddr, AllocByte, request.size)
                else if prevSize < request.size
                    memory.set(userAddr + prevSize, AllocByte, request.size - prevSize)

                // And set the new user address
                request.address = userAddr

                // Link to list of allocated blocks
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