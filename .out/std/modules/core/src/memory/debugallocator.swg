#global namespace memory
using swag, sync

public struct DebugAllocator
{
    allocator:  IAllocator

    first:  *void
    size:   uint    // Total user size, in bytes
    nextId: u32
    count:  u32     // Number of allocated block
    mutex:  Mutex
}

// This will be address before and after each allocated block
private
{
    const MagicAlloc = 0xC0DEC0DE   // Before and after the user memory
    const AllocByte  = 0xAB         // Fill memory when allocated
    const FreeByte   = 0xFB         // Fill memory when deleted

    // Will be put just before the returned address
    struct Header
    {
        allocAddr:      *void
        allocSize:       uint
        locFilename:    string
        hint:           string
        prev:           *Header
        next:           *Header
        userSize:       uint
        magic:          u32
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
    // Create the allocator
    func create(allocator: IAllocator = null)->DebugAllocator
    {
        var result: retval
        result.init(allocator)
        return result
    }

    // Initialize the allocator
    func init(using self, allocator: IAllocator = null)
    {
        self.allocator = #scopefct.allocator ?? @getcontext().allocator
    }

    // Returns true if there are some remaining allocations
    func isEmpty(using self) => first == null

    // Output to the console the list of all allocated blocks (leaks)
    func printAllocated(using self)
    {
        scan := cast(*Header) first
        console.printf("% block(s), % byte(s)\n", count, size)
        while scan
        {
            console.printf("id: % size: % hint: % loc: %:%\n", scan.allocId, scan.userSize, scan.hint, scan.locFilename, scan.locLine)
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
        debug.assert(ptrHeader.allocId != 0)
        debug.assert(ptrHeader.userSize != 0)
        debug.assert(ptrHeader.prev || first == ptrHeader)
        debug.assert(!ptrHeader.prev || ptrHeader.prev.next == ptrHeader)
        debug.assert(!ptrHeader.next || ptrHeader.next.prev == ptrHeader)
    }
}

public impl IAllocator for DebugAllocator
{
    func alloc(using self, request: *AllocatorRequest)
    {
        debug.assert(allocator != null, "not created")

        mutex.lock()
        defer mutex.unlock()

        switch request.mode
        {
        case Free:
            if !request.address return
            assertIsAllocated(request.address)

            userAddr := cast(*u8) request.address
            ptrHeader := cast(*Header) (userAddr - @sizeof(Header))
            ptrFooter := cast(*Footer) (userAddr + ptrHeader.userSize)

            // Be sure size is correct
            debug.assert(ptrHeader.userSize == request.size)

            // Unlink from allocated list
            if !ptrHeader.prev
                self.first = ptrHeader.next
            else
                ptrHeader.prev.next = ptrHeader.next
            if ptrHeader.next
                ptrHeader.next.prev = ptrHeader.prev

            // Stats
            debug.assert(self.count > 0)
            self.count -= 1
            debug.assert(self.size >= ptrHeader.userSize)
            userSize := ptrHeader.userSize
            self.size -= userSize

            // Fill the user memory area with garbage
            memory.set(userAddr, FreeByte, userSize)

            // Really free the memory
            copyReq := :request
            copyReq.address = ptrHeader.allocAddr
            copyReq.size = ptrHeader.allocSize
            allocator.alloc(&copyReq)

        case FreeAll:
            allocator.alloc(request)
            first = null
            count = 0
            size = 0

        case Realloc:
            assertIsAllocated(request.address)
            fallthrough

        case Alloc:
            // In case of a reallocation, get the previous allocated user size
            // This will be used to fill with garbage the difference
            copyReq := :request
            prevSize := 0'uint
            prevHeader := cast(*Header) request.address
            if request.mode == AllocatorMode.Realloc && request.address
            {
                prevHeader -= 1
                copyReq.address = prevHeader.allocAddr
                prevSize = prevHeader.userSize

                // Unlink now, before the realloc, because that block could
                // become invalid in case of a reallocation
                if prevHeader
                {
                    if !prevHeader.prev
                        self.first = prevHeader.next
                    else
                        prevHeader.prev.next = prevHeader.next
                    if prevHeader.next
                        prevHeader.next.prev = prevHeader.prev
                }
            }

            // Adapt size to be able to store additional informations
            // Take care of alignement request, because we need to be sure
            // that the returned memory block is well aligned
            toAddAlign := memory.align(@sizeof(Header), request.alignement)

            // Make the real allocation
            copyReq.size += toAddAlign + @sizeof(Footer)
            allocator.alloc(&copyReq)

            // This is the aligned returned user address
            userAddr := cast(*u8) copyReq.address
            userAddr += toAddAlign

            // Setup header
            header := cast(*Header) (userAddr - @sizeof(Header))
            header.magic = MagicAlloc
            header.allocAddr = copyReq.address
            header.allocSize = copyReq.size
            header.userSize = request.size
            header.locFilename = copyReq.callerLoc.fileName
            header.locLine = copyReq.callerLoc.lineStart + 1
            header.hint = copyReq.hint

            // Setup footer
            footer := cast(*Footer) (userAddr + request.size)
            footer.magic = MagicAlloc

            // Fill with garbage
            if request.mode == AllocatorMode.Alloc || !request.address
                memory.set(userAddr, AllocByte, request.size)
            else if prevSize < request.size
                memory.set(userAddr + prevSize, AllocByte, request.size - prevSize)

            // Stats
            if !prevSize self.count += 1
            self.size -= prevSize
            self.size += request.size

            // Set the new user address
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
        }
    }
}