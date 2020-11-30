namespace std.memory;
using swag

public struct DebugAllocator
{
    allocator:  IAllocator

    first:  *void
    nextId: u32
    count:  u32     // Number of allocated block
    size:   u32     // Total user size, in bytes
}

// This will be address before and after each allocated block
private
{
    const MagicAlloc = 0x12345678   // Before and after the user memory
    const MagicFree  = 0x87654321   // Before and after the user memory
    const AllocByte  = 0xAB         // Fill memory when allocated
    const FreeByte   = 0xFB         // Fill memory when deleted

    // Will be put just before the returned address
    struct Header
    {
        allocAddr:      *void
        locFilename:    string
        hint:           string
        prev:           *Header
        next:           *Header
        magic:          u32
        userSize:       u32
        allocId:        u32
        locLine:        u32
    }

    // Will be put just after the returned address
    struct Footer
    {
        magic:  u32
    }
}

public impl DebugAllocator
{
    // Returns true if there are some remaining allocations
    func isEmpty(using self) => first == null

    // Output to the console the list of all allocated blocks (leaks)
    func printAllocated(using self)
    {
        scan := cast(*Header) first
        console.print("% block(s), % byte(s)\n", count, size)
        while scan
        {
            console.print("id: % size: % hint: % loc: %:%\n", scan.allocId, scan.userSize, scan.hint, scan.locFilename, scan.locLine)
            scan = scan.next
        }
    }

    // This function will assert if the given address is not conform
    // to an allocated block
    func assertIsAllocated(using self, addr: *void)
    {
        if !addr return
        orgAddr := cast(*u8) addr

        ptrHeader := cast(*Header) (orgAddr - @sizeof(Header))
        ptrFooter := cast(*Footer) (orgAddr + ptrHeader.userSize)

        debug.assert(ptrHeader.magic == MagicAlloc)
        debug.assert(ptrFooter.magic == MagicAlloc)
        debug.assert(ptrHeader.prev || first == ptrHeader)
        debug.assert(!ptrHeader.prev || ptrHeader.prev.next == ptrHeader)
        debug.assert(!ptrHeader.next || ptrHeader.next.prev == ptrHeader)
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
                copyReq.address = ptrHeader.allocAddr
                allocator.alloc(&copyReq)

                // Fill the memory with garbage
                memory.set(orgAddr, FreeByte, ptrHeader.userSize)

            case AllocatorMode.FreeAll:
                allocator.alloc(request)
                self.first = null

            case AllocatorMode.Realloc:
                assertIsAllocated(request.address)
                if request.address self.count -= 1
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
                header.allocAddr = copyReq.address
                header.userSize = request.size
                header.locFilename = copyReq.callerLoc.fileName
                header.locLine = copyReq.callerLoc.lineStart
                header.hint = copyReq.hint

                // Setup footer
                footer := cast(*Footer) (orgAddr + request.size)
                footer.magic = MagicAlloc

                // Fill with garbage
                if request.mode == AllocatorMode.Alloc || !request.address
                    memory.set(userAddr, AllocByte, request.size)
                else if prevSize < request.size
                    memory.set(userAddr + prevSize, AllocByte, request.size - prevSize)

                // Stats
                self.count += 1
                self.size -= prevSize
                self.size += request.size

                // And set the new user address
                request.address = userAddr

                // Set a unique id
                self.nextId += 1
                header.allocId = self.nextId

                // Link to list of allocated blocks
                header.prev = null
                header.next = self.first
                if header.next
                    header.next.prev = header
                self.first = header
                break
        }
    }
}