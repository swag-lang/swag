#global namespace Swag

// Will be put just before the returned address
struct DebugAllocatorHeader
{
    loc:            SourceCodeLocation
    allocAddr:      ^void
    allocSize:      u64
    userSize:       u64
    hint:           string
    prev:           *DebugAllocatorHeader
    next:           *DebugAllocatorHeader
    stack:          [16] *void
    stackCount:     u32
    magic:          u32
    allocId:        u32
}

// Will be put just after the returned address
struct DebugAllocatorFooter
{
    magic: u32
}

struct DebugAllocator
{
    allocator:      IAllocator
    mutex:          MutexRW
    firstAlloc:     *DebugAllocatorHeader     // First allocated block
    firstFree:      *DebugAllocatorHeader     // First freed block
    lastFree:       *DebugAllocatorHeader     // Last freed block
    sizeAlloc:      u64                       // Total allocated memory, in bytes
    sizeFree:       u64                       // Total of free blocks in quarantine
    countAlloc:     u32                       // Number of allocated blocks
    nextId:         u32                       // The next allocated id to assign

    // Debug/behaviour parameters
    maxFreeSize:          u64 = 4 * 1024     // Maximum total size (in kbs) of free blocks in quarantine
    breakOnAllocId:       u32                // Will @assert when the current allocation id reaches that value
    showMaxLeaks:         u32 = 100          // Maximum number of memory leaks to show
    captureAllocStack     = true             // For each allocation, capture stack
    detectLeaks           = true             // Detect memory leaks if true
}

impl DebugAllocator
{
    const MagicAlloc = 0xC0DEC0DE     // Before and after the user memory
    const MagicFree  = 0xCAFECAFE     // Before and after the user memory
    const AllocByte  = 0xAB           // Fill memory when allocated
    const FreeByte   = 0xFB           // Fill memory when deleted

    func printDisplaySize(value: u64)
    {
        var sizeAlloc = value
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

    func memAlign(value: u64, alignment: u32)->u64
    {
        let toAlign = (alignment orelse #sizeof(*void)) - 1
        return (value + toAlign) & ~toAlign
    }

    mtd checkIsFreed(ptrHeader: ^DebugAllocatorHeader)
    {
        let ptrEnd    = cast(^u8) (ptrHeader + 1)
        let ptrFooter = cast(*DebugAllocatorFooter) (ptrEnd + ptrHeader.userSize)

        if (ptrHeader.magic != MagicFree or ptrFooter.magic != MagicFree)
        {
            me.mutex.unlock()
            @panic("memory block corruption", #curlocation)
        }

        if ptrHeader.allocId == 0 or
           ptrHeader.userSize == 0 or
           ptrHeader.stackCount > @countof(ptrHeader.stack) or
           (!ptrHeader.prev and me.firstFree != ptrHeader) or
           (ptrHeader.prev and ptrHeader.prev.next != ptrHeader) or
           (ptrHeader.next and ptrHeader.next.prev != ptrHeader)
        {
            me.mutex.unlock()
            @panic("memory block corruption", #curlocation)
        }
    }

    mtd checkIsAllocated(ptrHeader: ^DebugAllocatorHeader, callerLoc = #callerlocation)
    {
        let ptrEnd    = cast(^u8) (ptrHeader + 1)
        let ptrFooter = cast(*DebugAllocatorFooter) (ptrEnd + ptrHeader.userSize)

        if (ptrHeader.magic != MagicAlloc and ptrHeader.magic != MagicFree) or
           (ptrFooter.magic != MagicAlloc and ptrFooter.magic != MagicFree)
        {
            me.mutex.unlock()
            @panic("memory block corruption", callerLoc)
        }

        if ptrHeader.allocId == 0 or
           ptrHeader.userSize == 0 or
           ptrHeader.stackCount > @countof(ptrHeader.stack) or
           (ptrHeader.magic == MagicAlloc and !ptrHeader.prev and me.firstAlloc != ptrHeader) or
           (ptrHeader.magic == MagicFree and !ptrHeader.prev and me.firstFree != ptrHeader) or
           (ptrHeader.prev and ptrHeader.prev.next != ptrHeader) or
           (ptrHeader.next and ptrHeader.next.prev != ptrHeader)
        {
            me.mutex.unlock()
            @panic("memory block corruption", callerLoc)
        }

        if (ptrHeader.magic == MagicFree and ptrHeader.magic == MagicFree)
        {
            me.mutex.unlock()
            let loc = ptrHeader.loc.fileName ? ptrHeader.loc : callerLoc
            @panic("memory block double free", loc)
        }
    }
}

impl DebugAllocator
{
    mtd opDrop()
    {
        // Free all blocks in quarantine
        while me.sizeFree > 0:
            DebugAllocator.IAllocator.releaseLast(me)
    }

    // Setup the allocator
    mtd setup(allocator: IAllocator)
    {
        @assert(allocator != null)
        me.allocator = allocator
    }

    // Output to the console the list of all allocated blocks (leaks)
    mtd printLeaks()
    {
        if me.countAlloc == 0 or !me.detectLeaks:
            return

        var scan = cast(*DebugAllocatorHeader) me.firstAlloc

        @print("############################################\n")
        @print("Swag.DebugAllocator: Memory leaks detected !\n")
        @print("--------------------------------------------\n")
        @print(me.countAlloc, " block(s) for a total of ")
        printDisplaySize(me.sizeAlloc)
        @print("\n")
        @print("############################################\n")

        var cpt = 0
        while scan
        {
            me.checkIsAllocated(scan)

            // One leak
            @print("--------------------------------------------\n")
            @print("\x1b[91m")
            @print("leak error: ")
            @print("id [", scan.allocId, "] sizeAlloc [")
            printDisplaySize(scan.userSize)
            if scan.hint:
                @print("] hint [", scan.hint, "]")
            else:
                @print("]")
            @print("\n")
            @print("\x1b[36m")
            @print("--> ")
            @print(scan.loc.fileName, ":", scan.loc.lineStart + 1, "\n")
            @print("--------------------------------------------\n")

            // Stack
            if scan.stackCount
            {
                __logStackTrace(@mkslice(&scan.stack[0], scan.stackCount), &scan.loc, false)
            }

            // Next
            scan = scan.next

            // Clamp
            cpt += 1
            if me.showMaxLeaks and cpt >= me.showMaxLeaks
            {
                @print("--------------------------------------------\n")
                @print("\x1b[91m")
                @print("... too many leaks (")
                @print(me.countAlloc)
                @print(")...\n")
                @print("\x1b[97m")
                break
            }
        }

        @print("############################################\n")
    }

    // This function will assert if the given user address is not conform
    // to an allocated block
    mtd assertIsAllocated(addr: *void, callerLoc = #callerlocation)
    {
        if !addr:
            return
        let ptrHeader = cast(*DebugAllocatorHeader) (cast(^u8) addr - #sizeof(DebugAllocatorHeader))
        me.checkIsAllocated(ptrHeader, callerLoc)
    }

    // Check all allocated blocks
    mtd checkAllMemory()
    {
        var ptrHeader = cast(*DebugAllocatorHeader) me.firstAlloc
        while ptrHeader
        {
            me.checkIsAllocated(ptrHeader)
            ptrHeader = ptrHeader.next
        }

        ptrHeader = cast(*DebugAllocatorHeader) me.firstFree
        while ptrHeader
        {
            me.checkIsFreed(ptrHeader)
            ptrHeader = ptrHeader.next
        }
    }
}

impl IAllocator for DebugAllocator
{
    mtd free(request: *AllocatorRequest)
    {
        if !request.address:
            return

        me.assertIsAllocated(request.address, request.callerLoc)

        let userAddr  = cast(^u8) request.address
        let ptrHeader = cast(*DebugAllocatorHeader) (userAddr - #sizeof(DebugAllocatorHeader))
        let ptrFooter = cast(*DebugAllocatorFooter) (userAddr + ptrHeader.userSize)

        @assert(ptrHeader.userSize == request.size)

        // Unlink from allocated list
        if !ptrHeader.prev:
            me.firstAlloc = ptrHeader.next
        else:
            ptrHeader.prev.next = ptrHeader.next
        if ptrHeader.next:
            ptrHeader.next.prev = ptrHeader.prev

        // Stats
        @assert(me.countAlloc > 0)
        me.countAlloc -= 1
        @assert(me.sizeAlloc >= ptrHeader.userSize)
        let userSize = ptrHeader.userSize
        me.sizeAlloc -= userSize
        me.sizeFree += userSize

        // Fill the user memory area with garbage
        @memset(userAddr, FreeByte, userSize)

        // Change header/footer
        ptrHeader.magic = MagicFree
        ptrFooter.magic = MagicFree
        ptrHeader.loc   = request.callerLoc

        // This is the norm...
        request.address = null

        // Link to the start of the free list
        ptrHeader.prev = null
        ptrHeader.next = me.firstFree
        if me.firstFree:
            me.firstFree.prev = ptrHeader
        me.firstFree = ptrHeader
        if !me.lastFree:
            me.lastFree = me.firstFree

        // Purge oldest memory blocks if necessary, until we are in the quarantine
        // budget again
        while me.sizeFree > (me.maxFreeSize << 10):
            me.releaseLast()
    }

    mtd releaseLast()
    {
        if !me.lastFree:
            return

        let ptrHeader = me.lastFree
        me.checkIsFreed(ptrHeader)

        // Stats
        let userSize = ptrHeader.userSize
        @assert(userSize <= me.sizeFree)
        me.sizeFree -= userSize

        // Unlink from list
        if me.lastFree.prev
        {
            me.lastFree.prev.next = null
            me.lastFree           = me.lastFree.prev
        }
        else
        {
            @assert(me.firstFree == me.lastFree)
            @assert(me.firstFree == ptrHeader)
            me.lastFree, me.firstFree = null
        }

        let freeAddr = ptrHeader.allocAddr
        let freeSize = ptrHeader.allocSize

        // Fill header/footer with garbage
        let ptrFooter = cast(*DebugAllocatorFooter) (cast(^u8) ptrHeader.allocAddr + ptrHeader.userSize + #sizeof(DebugAllocatorHeader))
        @memset(ptrHeader, FreeByte, #sizeof(DebugAllocatorHeader))
        @memset(ptrFooter, FreeByte, #sizeof(DebugAllocatorFooter))

        // Really free the memory
        var req: AllocatorRequest
        req.mode    = .Free
        req.address = freeAddr
        req.size    = freeSize
        me.allocator.req(&req)
    }

    mtd alloc(request: *AllocatorRequest)
    {
        // Special case
        if request.size == 0
        {
            if request.mode == .Realloc:
                me.free(request)
            request.address = null
            return
        }

        // In case of a reallocation, get the previous allocated user sizeAlloc
        // This will be used to fill with garbage the difference
        var copyReq    = dref request
        var prevSize   = 0'u64
        var prevHeader = cast(^DebugAllocatorHeader) request.address
        if request.mode == .Realloc and request.address
        {
            prevHeader -= 1
            copyReq.address = prevHeader.allocAddr
            prevSize        = prevHeader.userSize

            // Unlink now, before the realloc, because that block could
            // become invalid in case of a reallocation
            if prevHeader
            {
                if !prevHeader.prev:
                    me.firstAlloc = prevHeader.next
                else:
                    prevHeader.prev.next = prevHeader.next
                if prevHeader.next:
                    prevHeader.next.prev = prevHeader.prev
            }
        }

        // Adapt sizeAlloc to be able to store additional informations
        // Take care of alignment request, because we need to be sure
        // that the returned memory block is well aligned
        let toAddAlign = memAlign(#sizeof(DebugAllocatorHeader), request.alignment)

        // Make the real allocation
        copyReq.size += toAddAlign + #sizeof(DebugAllocatorFooter)
        me.allocator.req(&copyReq)

        // This is the aligned returned user address
        var userAddr = cast(^u8) copyReq.address
        userAddr += toAddAlign

        // Setup header
        let header = cast(*DebugAllocatorHeader) (userAddr - #sizeof(DebugAllocatorHeader))
        header.magic      = MagicAlloc
        header.allocAddr  = copyReq.address
        header.allocSize  = copyReq.size
        header.userSize   = request.size
        header.loc        = copyReq.callerLoc
        header.hint       = copyReq.hint
        header.stackCount = me.captureAllocStack ? __captureStack(2, header.stack) : 0

        // Setup footer
        let footer = cast(*DebugAllocatorFooter) (userAddr + request.size)
        footer.magic = MagicAlloc

        // Fill with garbage
        if request.mode == .Alloc or !request.address:
            @memset(userAddr, AllocByte, request.size)
        elif prevSize < request.size:
            @memset(userAddr + prevSize, AllocByte, request.size - prevSize)

        // Stats
        if !prevSize:
            me.countAlloc += 1
        me.sizeAlloc -= prevSize
        me.sizeAlloc += request.size

        // Set the new user address
        request.address = userAddr

        // Set a unique id
        me.nextId += 1
        header.allocId = me.nextId

        // To but a breakpoint on a given allocator id
        if header.allocId == me.breakOnAllocId
        {
            @assert(false)
        }

        // Link to list of allocated blocks
        header.prev = null
        header.next = me.firstAlloc
        if header.next:
            header.next.prev = header
        me.firstAlloc = header
    }

    mtd impl req(request: *AllocatorRequest)
    {
        @assert(me.allocator != null)
        me.mutex.lock()
        defer me.mutex.unlock()

        switch request.mode
        {
        case Free:
            me.free(request)

        case FreeAll:
            me.allocator.req(request)
            me.firstAlloc = null
            me.countAlloc, me.sizeAlloc = 0

        case Realloc:
            me.assertIsAllocated(request.address, request.callerLoc)
            fallthrough

        case Alloc:
            me.alloc(request)

        case AssertIsAllocated:
            me.assertIsAllocated(request.address, request.callerLoc)
        }
    }
}
