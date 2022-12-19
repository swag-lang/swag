#global namespace Swag

// Will be put just before the returned address
struct DebugAllocatorHeader
{
    loc:            CompilerSourceLocation
    allocAddr:      ^void
    allocSize:      uint
    userSize:       uint
    hint:           string
    prev:           *DebugAllocatorHeader
    next:           *DebugAllocatorHeader
    magic:          u32
    allocId:        u32
}

// Will be put just after the returned address
struct DebugAllocatorFooter
{
    magic:  u32
}

struct DebugAllocator
{
    allocator:  IAllocator
    mutex:      MutexRW
    firstAlloc: *DebugAllocatorHeader   // First allocated block
    firstFree:  *DebugAllocatorHeader   // First freed block
    lastFree:   *DebugAllocatorHeader   // Last freed block
    sizeAlloc:  uint    // Total user sizeAlloc, in bytes
    countAlloc: u32     // Number of allocated blocks
    nextId:     u32     // The next allocated id to assign

    canPrintLeaks:  bool = true
}

impl DebugAllocator
{
    const MagicAlloc = 0xC0DEC0DE   // Before and after the user memory
    const MagicFree  = 0xCAFECAFE   // Before and after the user memory
    const AllocByte  = 0xAB         // Fill memory when allocated
    const FreeByte   = 0xFB         // Fill memory when deleted

    func printDisplaySize(value: uint)
    {
        sizeAlloc := value
        if sizeAlloc < 1024
        {
            @print(sizeAlloc, " bytes")
            return
        }

        sizeAlloc /= 1024
        if sizeAlloc < 1024
        {
            @print(sizeAlloc, " Kb")
            return
        }

        sizeAlloc /= 1024
        if sizeAlloc < 1024
        {
            @print(sizeAlloc, " Mb")
            return
        }

        sizeAlloc /= 1024
        if sizeAlloc < 1024
        {
            @print(sizeAlloc, " Gb")
            return
        }

        sizeAlloc /= 1024
        @print(sizeAlloc, " Tb")
    }

    func memAlign(value: uint, alignement: u32)->uint
    {
        toAlign := alignement orelse @sizeof(*void)
        result := value & ~(toAlign - 1)
        if result < value result += toAlign
        return result
    }

    mtd checkIsAllocated(ptrHeader: ^DebugAllocatorHeader)
    {
        ptrEnd := cast(^u8) (ptrHeader + 1)
        ptrFooter := cast(*DebugAllocatorFooter) (ptrEnd + ptrHeader.userSize)

        if (ptrHeader.magic != MagicAlloc and ptrHeader.magic != MagicFree) or
           (ptrFooter.magic != MagicAlloc and ptrFooter.magic != MagicFree)
        {
            @panic("block is corrupt", #location)
        }

        if ptrHeader.allocId == 0 or
           ptrHeader.userSize == 0 or
           (ptrHeader.magic == MagicAlloc and !ptrHeader.prev and firstAlloc != ptrHeader) or
           (ptrHeader.magic == MagicFree and !ptrHeader.prev and firstFree != ptrHeader) or
           (ptrHeader.prev and ptrHeader.prev.next != ptrHeader) or
           (ptrHeader.next and ptrHeader.next.prev != ptrHeader)
        {
            @panic("block is corrupt", #location)
        }

        if (ptrHeader.magic == MagicFree and ptrHeader.magic == MagicFree)
        {
            loc := ptrHeader.loc.fileName ? ptrHeader.loc : #location
            @panic("double free", loc)
        }
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

    // Output to the console the list of all allocated blocks (leaks)
    mtd printAllocated()
    {
        scan := cast(*DebugAllocatorHeader) firstAlloc
        @print(countAlloc, " block(s) for a total of ")
        printDisplaySize(sizeAlloc)
        @print("\n")

        while scan
        {
            @print(scan.loc.fileName, ":", scan.loc.lineStart + 1, ": ")
            @print("id: ", scan.allocId, " sizeAlloc: ")
            printDisplaySize(scan.userSize)
            if scan.hint @print(" hint: ", scan.hint)
            @print("\n")

            scan = scan.next
        }
    }

    // Output to the console the list of all allocated blocks (leaks)
    mtd printLeaks()
    {
        if countAlloc == 0 or !canPrintLeaks
            return
        @print("############################################\n")
        @print("Swag.DebugAllocator: Memory leaks detected !\n")
        @print("############################################\n")
        printAllocated()
    }

    // This function will assert if the given user address is not conform
    // to an allocated block
    mtd assertIsAllocated(addr: *void)
    {
        if !addr
            return
        ptrHeader := cast(*DebugAllocatorHeader) (cast(^u8) addr - @sizeof(DebugAllocatorHeader))
        checkIsAllocated(ptrHeader)
    }

    // Check all allocated pointers
    mtd checkAllAllocated()
    {
        ptrHeader := cast(*DebugAllocatorHeader) firstAlloc
        while ptrHeader
        {
            checkIsAllocated(ptrHeader)
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
        ptrHeader := cast(*DebugAllocatorHeader) (userAddr - @sizeof(DebugAllocatorHeader))
        ptrFooter := cast(*DebugAllocatorFooter) (userAddr + ptrHeader.userSize)

        @assert(ptrHeader.userSize == request.size)

        // Unlink from allocated list
        if !ptrHeader.prev
            self.firstAlloc = ptrHeader.next
        else
            ptrHeader.prev.next = ptrHeader.next
        if ptrHeader.next
            ptrHeader.next.prev = ptrHeader.prev

        // Link to free list
        ptrHeader.prev = null
        ptrHeader.next = firstFree
        if firstFree
            firstFree.prev = ptrHeader
        firstFree = ptrHeader
        if !lastFree
            lastFree = firstFree

        // Stats
        @assert(self.countAlloc > 0)
        self.countAlloc -= 1
        @assert(self.sizeAlloc >= ptrHeader.userSize)
        userSize := ptrHeader.userSize
        self.sizeAlloc -= userSize

        // Fill the user memory area with garbage
        @memset(userAddr, FreeByte, userSize)

        // Change header/footer
        ptrHeader.magic = MagicFree
        ptrFooter.magic = MagicFree
        ptrHeader.loc = request.callerLoc

        // This is the norm...
        request.address = null

        releaseLast()
    }

    mtd releaseLast()
    {
        if !lastFree
            return

        // Unlink from list
        ptrHeader := lastFree
        if lastFree.prev
        {
            lastFree.prev.next = null
            lastFree = lastFree.prev
        }
        else
        {
            @assert(firstFree == lastFree)
            @assert(firstFree == ptrHeader)
            lastFree, firstFree = null
        }

        freeAddr := ptrHeader.allocAddr
        freeSize := ptrHeader.allocSize

        // Fill header/footer
        ptrFooter := cast(*DebugAllocatorFooter) (ptrHeader.allocAddr + ptrHeader.userSize + @sizeof(DebugAllocatorHeader))
        @memset(ptrHeader, FreeByte, @sizeof(DebugAllocatorHeader))
        @memset(ptrFooter, FreeByte, @sizeof(DebugAllocatorFooter))

        // Really free the memory
        var req: AllocatorRequest
        req.mode = .Free
        req.address = freeAddr
        req.size = freeSize
        allocator.req(&req)
    }

    mtd alloc(request: *AllocatorRequest)
    {
        // Special case
        if request.size == 0
        {
            if request.mode == .Realloc
                free(request)
            request.address = null
            return
        }

        // In case of a reallocation, get the previous allocated user sizeAlloc
        // This will be used to fill with garbage the difference
        copyReq := dref request
        prevSize := 0'uint
        prevHeader := cast(^DebugAllocatorHeader) request.address
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

        // Adapt sizeAlloc to be able to store additional informations
        // Take care of alignement request, because we need to be sure
        // that the returned memory block is well aligned
        toAddAlign := memAlign(@sizeof(DebugAllocatorHeader), request.alignement)

        // Make the real allocation
        copyReq.size += toAddAlign + @sizeof(DebugAllocatorFooter)
        allocator.req(&copyReq)

        // This is the aligned returned user address
        userAddr := cast(^u8) copyReq.address
        userAddr += toAddAlign

        // Setup header
        header := cast(*DebugAllocatorHeader) (userAddr - @sizeof(DebugAllocatorHeader))
        header.magic = MagicAlloc
        header.allocAddr = copyReq.address
        header.allocSize = copyReq.size
        header.userSize = request.size
        header.loc = copyReq.callerLoc
        header.hint = copyReq.hint

        // Setup footer
        footer := cast(*DebugAllocatorFooter) (userAddr + request.size)
        footer.magic = MagicAlloc

        // Fill with garbage
        if request.mode == AllocatorMode.Alloc or !request.address
            @memset(userAddr, AllocByte, request.size)
        elif prevSize < request.size
            @memset(userAddr + prevSize, AllocByte, request.size - prevSize)

        // Stats
        if !prevSize
            self.countAlloc += 1
        self.sizeAlloc -= prevSize
        self.sizeAlloc += request.size

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
            countAlloc, sizeAlloc = 0

        case Realloc:
            assertIsAllocated(request.address)
            fallthrough

        case Alloc:
            alloc(request)
        }
    }
}
