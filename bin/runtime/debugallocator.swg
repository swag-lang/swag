#global namespace Swag

struct DebugAllocator
{
    allocator:  IAllocator
    firstAlloc: *void   // First allocated block
    size:       uint    // Total user size, in bytes
    nextId:     u32     // The next allocated id to assign
    count:      u32     // Number of allocated blocks
    mutex:      MutexRW
}

// This will be address before and after each allocated block
impl DebugAllocator
{
    const MagicAlloc = 0xC0DEC0DE   // Before and after the user memory
    const AllocByte  = 0xAB         // Fill memory when allocated
    const FreeByte   = 0xFB         // Fill memory when deleted

    // Will be put just before the returned address
    struct Header
    {
        allocAddr:      ^void
        allocSize:      uint
        userSize:       uint
        locFilename:    string
        hint:           string
        prev:           *Header
        next:           *Header
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

impl DebugAllocator
{
    func printDisplaySize(value: uint)
    {
        size := value
        if size < 1024
        {
            @print(size, " bytes")
            return
        }

        size /= 1024
        if size < 1024
        {
            @print(size, " Kb")
            return
        }

        size /= 1024
        if size < 1024
        {
            @print(size, " Mb")
            return
        }

        size /= 1024
        if size < 1024
        {
            @print(size, " Gb")
            return
        }

        size /= 1024
        @print(size, " Tb")
    }

    func memAlign(value: uint, alignement: u32)->uint
    {
        toAlign := alignement orelse @sizeof(*void)
        result := value & ~(toAlign - 1)
        if result < value result += toAlign
        return result
    }

    mtd checkOne(ptrHeader: ^Header)
    {
        ptrEnd := cast(^u8) (ptrHeader + 1)
        ptrFooter := cast(*Footer) (ptrEnd + ptrHeader.userSize)

        @assert(ptrHeader.magic == MagicAlloc)
        @assert(ptrFooter.magic == MagicAlloc)
        @assert(ptrHeader.allocId != 0)
        @assert(ptrHeader.userSize != 0)
        @assert(ptrHeader.prev or firstAlloc == ptrHeader)
        @assert(!ptrHeader.prev or ptrHeader.prev.next == ptrHeader)
        @assert(!ptrHeader.next or ptrHeader.next.prev == ptrHeader)
    }
}

impl DebugAllocator
{
    // Setup the allocator
    mtd setup(allocator: IAllocator)
    {
        @assert(#self.allocator != null)
        self.allocator = #self.allocator
    }

    // Returns true if there are some remaining allocations
    mtd isEmpty() => firstAlloc == null

    // Output to the console the list of all allocated blocks (leaks)
    mtd printAllocated()
    {
        scan := cast(*Header) firstAlloc
        @print(count, " block(s) for a total of ")
        printDisplaySize(size)
        @print("\n")

        while scan
        {
            @print(scan.locFilename, ":", scan.locLine, ": ")
            @print("id: ", scan.allocId, " size: ")
            printDisplaySize(scan.userSize)
            if scan.hint @print(" hint: ", scan.hint)
            @print("\n")

            scan = scan.next
        }
    }

    // Output to the console the list of all allocated blocks (leaks)
    mtd printLeaks()
    {
        if isEmpty()
            return
        @print("############################################\n")
        @print("Swag.DebugAllocator: Memory leaks detected !\n")
        @print("############################################\n")
        printAllocated()
    }

    // This function will assert if the given address is not conform
    // to an allocated block
    //#[Swag.PrintBc]
    mtd assertIsAllocated(addr: *void)
    {
        if !addr return

        orgAddr := cast(^u8) addr
        ptrHeader := cast(*Header) (orgAddr - @sizeof(Header))
        checkOne(ptrHeader)
    }

    // Check all allocated pointers
    mtd checkAll()
    {
        ptrHeader := cast(*Header) firstAlloc
        while ptrHeader
        {
            checkOne(ptrHeader)
            ptrHeader = ptrHeader.next
        }
    }
}

impl IAllocator for DebugAllocator
{
    mtd free(request: *AllocatorRequest)
    {
        if !request.address
            return
        assertIsAllocated(request.address)

        userAddr := cast(^u8) request.address
        ptrHeader := cast(*Header) (userAddr - @sizeof(Header))
        ptrFooter := cast(*Footer) (userAddr + ptrHeader.userSize)

        @assert(ptrHeader.userSize == request.size)

        // Unlink from allocated list
        if !ptrHeader.prev
            self.firstAlloc = ptrHeader.next
        else
            ptrHeader.prev.next = ptrHeader.next
        if ptrHeader.next
            ptrHeader.next.prev = ptrHeader.prev

        // Stats
        @assert(self.count > 0)
        self.count -= 1
        @assert(self.size >= ptrHeader.userSize)
        userSize := ptrHeader.userSize
        self.size -= userSize

        freeAddr := ptrHeader.allocAddr
        freeSize := ptrHeader.allocSize

        // Fill the memory area with garbage
        @memset(userAddr, FreeByte, userSize)
        @memset(ptrHeader, FreeByte, @sizeof(Header))
        @memset(ptrFooter, FreeByte, @sizeof(Footer))

        // Really free the memory
        var req: AllocatorRequest
        req.mode = .Free
        req.address = freeAddr
        req.size = freeSize
        allocator.req(&req)
        request.address = null
    }

    mtd req(request: *AllocatorRequest)
    {
        @assert(allocator != null)
        mutex.lock()
        defer mutex.unlock()

        switch request.mode
        {
        case Free:
            free(request)

        case FreeAll:
            allocator.req(request)
            firstAlloc = null
            count = 0
            size = 0

        case Realloc:
            assertIsAllocated(request.address)
            fallthrough

        case Alloc:
            // Special case
            if request.size == 0
            {
                if request.mode == .Realloc
                    free(request)
                request.address = null
                return
            }

            // In case of a reallocation, get the previous allocated user size
            // This will be used to fill with garbage the difference
            copyReq := dref request
            prevSize := 0'uint
            prevHeader := cast(^Header) request.address
            if request.mode == AllocatorMode.Realloc and request.address
            {
                prevHeader -= 1
                copyReq.address = prevHeader.allocAddr
                prevSize = prevHeader.userSize

                // Unlink now, before the realloc, because that block could
                // become invalid in case of a reallocation
                if prevHeader
                {
                    if !prevHeader.prev
                        self.firstAlloc = prevHeader.next
                    else
                        prevHeader.prev.next = prevHeader.next
                    if prevHeader.next
                        prevHeader.next.prev = prevHeader.prev
                }
            }

            // Adapt size to be able to store additional informations
            // Take care of alignement request, because we need to be sure
            // that the returned memory block is well aligned
            toAddAlign := memAlign(@sizeof(Header), request.alignement)

            // Make the real allocation
            copyReq.size += toAddAlign + @sizeof(Footer)
            allocator.req(&copyReq)

            // This is the aligned returned user address
            userAddr := cast(^u8) copyReq.address
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
            if request.mode == AllocatorMode.Alloc or !request.address
                @memset(userAddr, AllocByte, request.size)
            elif prevSize < request.size
                @memset(userAddr + prevSize, AllocByte, request.size - prevSize)

            // Stats
            if !prevSize
                self.count += 1
            self.size -= prevSize
            self.size += request.size

            // Set the new user address
            request.address = userAddr

            // Set a unique id
            self.nextId += 1
            header.allocId = self.nextId

            // To but a breakpoint on a given allocator id
            #[Swag.Global]
            var breakId: u32
            if header.allocId == breakId
            {
                @assert(false)
            }

            // Link to list of allocated blocks
            header.prev = null
            header.next = self.firstAlloc
            if header.next
                header.next.prev = header
            self.firstAlloc = header
        }
    }
}
