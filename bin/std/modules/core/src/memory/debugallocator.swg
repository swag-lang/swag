#global namespace Memory
using Swag, Sync

public struct DebugAllocator
{
    allocator:  IAllocator

    first:      *void
    size:       uint    // Total user size, in bytes
    nextId:     u32
    count:      u32     // Number of allocated block
    mutex:      Mutex
    traceLeaks: bool
}

// This will be address before and after each allocated block
private impl DebugAllocator
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
    func create(allocator: IAllocator = null, traceLeaks = true)->DebugAllocator
    {
        var result: retval
        result.init(allocator, traceLeaks)
        return result
    }

    // Initialize the allocator
    mtd init(allocator: IAllocator = null, traceLeaks = true)
    {
        self.allocator = #self.allocator orelse @getcontext().allocator
        self.traceLeaks = #self.traceLeaks
    }

    // Returns true if there are some remaining allocations
    mtd isEmpty() => first == null

    // Output to the console the list of all allocated blocks (leaks)
    mtd printAllocated()
    {
        scan := cast(*Header) first
        Console.printf("% block(s), %\n", count, StrConv.toDisplaySize(size))
        Console.printf("\n")
        while scan
        {
            Console.printf("id: % size: %\n", scan.allocId, StrConv.toDisplaySize(scan.userSize))
            Console.printf("    hint: %\n", scan.hint)
            Console.printf("    loc: %:%\n", scan.locFilename, scan.locLine)
            scan = scan.next
        }
    }

    // This function will assert if the given address is not conform
    // to an allocated block
    mtd assertIsAllocated(addr: *void)
    {
        if !addr return

        orgAddr := cast(*u8) addr
        ptrHeader := cast(*Header) (orgAddr - @sizeof(Header))
        checkOne(ptrHeader)
    }

    private mtd checkOne(ptrHeader: *Header)
    {
        ptrEnd := cast(*u8) (ptrHeader + 1)
        ptrFooter := cast(*Footer) (ptrEnd + ptrHeader.userSize)

        Debug.assert(ptrHeader.magic == MagicAlloc)
        Debug.assert(ptrFooter.magic == MagicAlloc)
        Debug.assert(ptrHeader.allocId != 0)
        Debug.assert(ptrHeader.userSize != 0)
        Debug.assert(ptrHeader.prev or first == ptrHeader)
        Debug.assert(!ptrHeader.prev or ptrHeader.prev.next == ptrHeader)
        Debug.assert(!ptrHeader.next or ptrHeader.next.prev == ptrHeader)
    }

    // Check all allocated pointers
    mtd checkAll()
    {
        ptrHeader := cast(*Header) first
        while ptrHeader
        {
            checkOne(ptrHeader)
            ptrHeader = ptrHeader.next
        }
    }

    // Install debug allocator as the default one
    func install(traceLeaks = true)
    {
        #[Swag.Global]
        var dbg: DebugAllocator
        dbg = create(traceLeaks: traceLeaks)
        @getcontext().allocator = dbg
    }

    // Check all allocated pointers taking the current contextual allocator
    func cxtCheckAll()
    {
        y := @getcontext().allocator
        x := @kindof(y)
        if x == Memory.DebugAllocator
        {
            i := cast(*Memory.DebugAllocator) @dataof(@getcontext().allocator)
            i.checkAll()
        }
    }
}

public impl IAllocator for DebugAllocator
{
    func alloc(using self, request: *AllocatorRequest)
    {
        Debug.assert(allocator != null, "not created")

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

            Debug.assert(ptrHeader.userSize == request.size)

            // Unlink from allocated list
            if !ptrHeader.prev
                self.first = ptrHeader.next
            else
                ptrHeader.prev.next = ptrHeader.next
            if ptrHeader.next
                ptrHeader.next.prev = ptrHeader.prev

            // Stats
            Debug.assert(self.count > 0)
            self.count -= 1
            Debug.assert(self.size >= ptrHeader.userSize)
            userSize := ptrHeader.userSize
            self.size -= userSize

            freeAddr := ptrHeader.allocAddr
            freeSize := ptrHeader.allocSize

            // Fill the memory area with garbage
            Memory.set(userAddr, FreeByte, userSize)
            Memory.set(ptrHeader, FreeByte, @sizeof(Header))
            Memory.set(ptrFooter, FreeByte, @sizeof(Footer))

            // Really free the memory
            copyReq := dref request
            copyReq.address = freeAddr
            copyReq.size = freeSize
            allocator.alloc(&copyReq)

            //checkAll()

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
            copyReq := dref request
            prevSize := 0'uint
            prevHeader := cast(*Header) request.address
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
            toAddAlign := Memory.align(@sizeof(Header), request.alignement)

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
            if request.mode == AllocatorMode.Alloc or !request.address
                Memory.set(userAddr, AllocByte, request.size)
            elif prevSize < request.size
                Memory.set(userAddr + prevSize, AllocByte, request.size - prevSize)

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
                Debug.assert(false)
            }

            // Link to list of allocated blocks
            header.prev = null
            header.next = self.first
            if header.next
                header.next.prev = header
            self.first = header

            //checkAll()
        }
    }
}

#drop
{
    if @kindof(@getcontext().allocator) == DebugAllocator
    {
        @getcontext().tempAllocator.release()
        dbg := cast(*DebugAllocator) @dataof(@getcontext().allocator)
        if dbg.traceLeaks and !dbg.isEmpty()
        {
            Console.print("#######################################\n")
            Console.print("DebugAllocator: Memory leaks detected !\n")
            Console.print("#######################################\n")
            dbg.printAllocated()
        }
    }
}