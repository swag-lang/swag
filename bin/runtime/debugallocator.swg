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
    func __printDisplaySize(value: uint)
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

    func __memAlign(value: uint, alignement: u32)->uint
    {
        toAlign := alignement orelse @sizeof(*void)
        result := value & ~(toAlign - 1)
        if result < value result += toAlign
        return result
    }

    mtd __checkOne(ptrHeader: ^Header)
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
    // Create the allocator, passing the "real one" as parameter
    func create(allocator: IAllocator = null)->DebugAllocator
    {
        var result: retval
        result.init(allocator)
        return result
    }

    // Initialize the allocator with the "real one" as parameter
    mtd init(allocator: IAllocator = null)
    {
        self.allocator = #self.allocator orelse @getcontext().allocator
    }

    // Returns true if there are some remaining allocations
    mtd isEmpty() => firstAlloc == null

    // Output to the console the list of all allocated blocks (leaks)
    mtd printAllocated()
    {
        scan := cast(*Header) firstAlloc
        @print(count, " block(s) for a total of ")
        __printDisplaySize(size)
        @print("\n")

        while scan
        {
            @print(scan.locFilename, ":", scan.locLine, ": ")
            @print("id: ", scan.allocId, " size: ")
            __printDisplaySize(scan.userSize)
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
    mtd assertIsAllocated(addr: *void)
    {
        if !addr return

        orgAddr := cast(^u8) addr
        ptrHeader := cast(*Header) (orgAddr - @sizeof(Header))
        __checkOne(ptrHeader)
    }

    // Check all allocated pointers
    mtd checkAll()
    {
        ptrHeader := cast(*Header) firstAlloc
        while ptrHeader
        {
            __checkOne(ptrHeader)
            ptrHeader = ptrHeader.next
        }
    }

    // Install debug allocator as the default one
    func install()
    {
        #[Swag.Global]
        var dbg: DebugAllocator
        dbg = create()
        @getcontext().allocator = dbg
    }
}

impl IAllocator for DebugAllocator
{
    func alloc(using self, request: *AllocatorRequest)
    {
        @assert(allocator != null)

        mutex.lock()
        defer mutex.unlock()

        switch request.mode
        {
        case Free:
            if !request.address return
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
            copyReq := dref request
            copyReq.address = freeAddr
            copyReq.size = freeSize
            allocator.alloc(&copyReq)

        case FreeAll:
            allocator.alloc(request)
            firstAlloc = null
            count = 0
            size = 0

        case Realloc:
            assertIsAllocated(request.address)
            fallthrough

        case Alloc:
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
            toAddAlign := __memAlign(@sizeof(Header), request.alignement)

            // Make the real allocation
            copyReq.size += toAddAlign + @sizeof(Footer)
            allocator.alloc(&copyReq)

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
