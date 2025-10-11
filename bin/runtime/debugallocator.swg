#global namespace Swag

// Will be put just before the returned address
struct DebugAllocatorHeader
{
    loc:            SourceCodeLocation
    allocAddr:      [*] void
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

    mtd checkIsFreed(ptrHeader: [*] DebugAllocatorHeader)
    {
        let ptrEnd    = cast([*] u8) (ptrHeader + 1)
        let ptrFooter = cast(*DebugAllocatorFooter) (ptrEnd + ptrHeader.userSize)

        if (ptrHeader.magic != MagicFree or ptrFooter.magic != MagicFree)
        {
            .mutex.unlock()
            @panic("memory block corruption", #curlocation)
        }

        if ptrHeader.allocId == 0 or
           ptrHeader.userSize == 0 or
           ptrHeader.stackCount > @countof(ptrHeader.stack) or
           (!ptrHeader.prev and .firstFree != ptrHeader) or
           (ptrHeader.prev and ptrHeader.prev.next != ptrHeader) or
           (ptrHeader.next and ptrHeader.next.prev != ptrHeader)
        {
            .mutex.unlock()
            @panic("memory block corruption", #curlocation)
        }
    }

    mtd checkIsAllocated(ptrHeader: [*] DebugAllocatorHeader, callerLoc = #callerlocation)
    {
        let ptrEnd    = cast([*] u8) (ptrHeader + 1)
        let ptrFooter = cast(*DebugAllocatorFooter) (ptrEnd + ptrHeader.userSize)

        if (ptrHeader.magic != MagicAlloc and ptrHeader.magic != MagicFree) or
           (ptrFooter.magic != MagicAlloc and ptrFooter.magic != MagicFree)
        {
            .mutex.unlock()
            @panic("memory block corruption", callerLoc)
        }

        if ptrHeader.allocId == 0 or
           ptrHeader.userSize == 0 or
           ptrHeader.stackCount > @countof(ptrHeader.stack) or
           (ptrHeader.magic == MagicAlloc and !ptrHeader.prev and .firstAlloc != ptrHeader) or
           (ptrHeader.magic == MagicFree and !ptrHeader.prev and .firstFree != ptrHeader) or
           (ptrHeader.prev and ptrHeader.prev.next != ptrHeader) or
           (ptrHeader.next and ptrHeader.next.prev != ptrHeader)
        {
            .mutex.unlock()
            @panic("memory block corruption", callerLoc)
        }

        if (ptrHeader.magic == MagicFree and ptrHeader.magic == MagicFree)
        {
            .mutex.unlock()
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
        while .sizeFree > 0 do
            DebugAllocator.IAllocator.releaseLast(me)
    }

    // Setup the allocator
    mtd setup(allocator: IAllocator)
    {
        @assert(allocator != null)
        .allocator = allocator
    }

    // Output to the console the list of all allocated blocks (leaks)
    mtd printLeaks()
    {
        if .countAlloc == 0 or !.detectLeaks do
            return

        var scan = cast(*DebugAllocatorHeader) .firstAlloc

        @print("############################################\n")
        @print("Swag.DebugAllocator: Memory leaks detected !\n")
        @print("--------------------------------------------\n")
        @print(.countAlloc, " block(s) for a total of ")
        printDisplaySize(.sizeAlloc)
        @print("\n")
        @print("############################################\n")

        var cpt = 0
        while scan
        {
            .checkIsAllocated(scan)

            // One leak
            @print("--------------------------------------------\n")
            @print("\x1b[91m")
            @print("leak error: ")
            @print("id [", scan.allocId, "] sizeAlloc [")
            printDisplaySize(scan.userSize)
            if scan.hint do
                @print("] hint [", scan.hint, "]")
            else do
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
            if .showMaxLeaks and cpt >= .showMaxLeaks
            {
                @print("--------------------------------------------\n")
                @print("\x1b[91m")
                @print("... too many leaks (")
                @print(.countAlloc)
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
        if !addr do
            return
        let ptrHeader = cast(*DebugAllocatorHeader) (cast([*] u8) addr - #sizeof(DebugAllocatorHeader))
        .checkIsAllocated(ptrHeader, callerLoc)
    }

    // Check all allocated blocks
    mtd checkAllMemory()
    {
        var ptrHeader = cast(*DebugAllocatorHeader) .firstAlloc
        while ptrHeader
        {
            .checkIsAllocated(ptrHeader)
            ptrHeader = ptrHeader.next
        }

        ptrHeader = cast(*DebugAllocatorHeader) .firstFree
        while ptrHeader
        {
            .checkIsFreed(ptrHeader)
            ptrHeader = ptrHeader.next
        }
    }
}

impl IAllocator for DebugAllocator
{
    mtd free(request: *AllocatorRequest)
    {
        if !request.address do
            return

        .assertIsAllocated(request.address, request.callerLoc)

        let userAddr  = cast([*] u8) request.address
        let ptrHeader = cast(*DebugAllocatorHeader) (userAddr - #sizeof(DebugAllocatorHeader))
        let ptrFooter = cast(*DebugAllocatorFooter) (userAddr + ptrHeader.userSize)

        @assert(ptrHeader.userSize == request.size)

        // Unlink from allocated list
        if !ptrHeader.prev do
            .firstAlloc = ptrHeader.next
        else do
            ptrHeader.prev.next = ptrHeader.next
        if ptrHeader.next do
            ptrHeader.next.prev = ptrHeader.prev

        // Stats
        @assert(.countAlloc > 0)
        .countAlloc -= 1
        @assert(.sizeAlloc >= ptrHeader.userSize)
        let userSize = ptrHeader.userSize
        .sizeAlloc -= userSize
        .sizeFree += userSize

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
        ptrHeader.next = .firstFree
        if .firstFree do
            .firstFree.prev = ptrHeader
        .firstFree = ptrHeader
        if !.lastFree do
            .lastFree = .firstFree

        // Purge oldest memory blocks if necessary, until we are in the quarantine
        // budget again
        while .sizeFree > (.maxFreeSize << 10) do
            .releaseLast()
    }

    mtd releaseLast()
    {
        if !.lastFree do
            return

        let ptrHeader = .lastFree
        .checkIsFreed(ptrHeader)

        // Stats
        let userSize = ptrHeader.userSize
        @assert(userSize <= .sizeFree)
        .sizeFree -= userSize

        // Unlink from list
        if .lastFree.prev
        {
            .lastFree.prev.next = null
            .lastFree           = .lastFree.prev
        }
        else
        {
            @assert(.firstFree == .lastFree)
            @assert(.firstFree == ptrHeader)
            .lastFree, .firstFree = null
        }

        let freeAddr = ptrHeader.allocAddr
        let freeSize = ptrHeader.allocSize

        // Fill header/footer with garbage
        let ptrFooter = cast(*DebugAllocatorFooter) (cast([*] u8) ptrHeader.allocAddr + ptrHeader.userSize + #sizeof(DebugAllocatorHeader))
        @memset(ptrHeader, FreeByte, #sizeof(DebugAllocatorHeader))
        @memset(ptrFooter, FreeByte, #sizeof(DebugAllocatorFooter))

        // Really free the memory
        var req: AllocatorRequest
        req.mode    = .Free
        req.address = freeAddr
        req.size    = freeSize
        .allocator.req(&req)
    }

    mtd alloc(request: *AllocatorRequest)
    {
        // Special case
        if request.size == 0
        {
            if request.mode == .Realloc do
                .free(request)
            request.address = null
            return
        }

        // In case of a reallocation, get the previous allocated user sizeAlloc
        // This will be used to fill with garbage the difference
        var copyReq    = dref request
        var prevSize   = 0'u64
        var prevHeader = cast([*] DebugAllocatorHeader) request.address
        if request.mode == .Realloc and request.address
        {
            prevHeader -= 1
            copyReq.address = prevHeader.allocAddr
            prevSize        = prevHeader.userSize

            // Unlink now, before the realloc, because that block could
            // become invalid in case of a reallocation
            if prevHeader
            {
                if !prevHeader.prev do
                    .firstAlloc = prevHeader.next
                else do
                    prevHeader.prev.next = prevHeader.next
                if prevHeader.next do
                    prevHeader.next.prev = prevHeader.prev
            }
        }

        // Adapt sizeAlloc to be able to store additional informations
        // Take care of alignment request, because we need to be sure
        // that the returned memory block is well aligned
        let toAddAlign = memAlign(#sizeof(DebugAllocatorHeader), request.alignment)

        // Make the real allocation
        copyReq.size += toAddAlign + #sizeof(DebugAllocatorFooter)
        .allocator.req(&copyReq)

        // This is the aligned returned user address
        var userAddr = cast([*] u8) copyReq.address
        userAddr += toAddAlign

        // Setup header
        let header = cast(*DebugAllocatorHeader) (userAddr - #sizeof(DebugAllocatorHeader))
        header.magic      = MagicAlloc
        header.allocAddr  = copyReq.address
        header.allocSize  = copyReq.size
        header.userSize   = request.size
        header.loc        = copyReq.callerLoc
        header.hint       = copyReq.hint
        header.stackCount = .captureAllocStack ? __captureStack(2, header.stack) : 0

        // Setup footer
        let footer = cast(*DebugAllocatorFooter) (userAddr + request.size)
        footer.magic = MagicAlloc

        // Fill with garbage
        if request.mode == .Alloc or !request.address do
            @memset(userAddr, AllocByte, request.size)
        elif prevSize < request.size do
            @memset(userAddr + prevSize, AllocByte, request.size - prevSize)

        // Stats
        if !prevSize do
            .countAlloc += 1
        .sizeAlloc -= prevSize
        .sizeAlloc += request.size

        // Set the new user address
        request.address = userAddr

        // Set a unique id
        .nextId += 1
        header.allocId = .nextId

        // To but a breakpoint on a given allocator id
        if header.allocId == .breakOnAllocId
        {
            @assert(false)
        }

        // Link to list of allocated blocks
        header.prev = null
        header.next = .firstAlloc
        if header.next do
            header.next.prev = header
        .firstAlloc = header
    }

    mtd impl req(request: *AllocatorRequest)
    {
        @assert(.allocator != null)
        .mutex.lock()
        defer .mutex.unlock()

        switch request.mode
        {
        case Free:
            .free(request)

        case FreeAll:
            .allocator.req(request)
            .firstAlloc = null
            .countAlloc, .sizeAlloc = 0

        case Realloc:
            .assertIsAllocated(request.address, request.callerLoc)
            fallthrough

        case Alloc:
            .alloc(request)

        case AssertIsAllocated:
            .assertIsAllocated(request.address, request.callerLoc)
        }
    }
}
