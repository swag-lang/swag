#global public

#[Swag.EnumFlags]
enum LogFlags
{
    Zero = 0

    Prefix
    Date
    Time
    ShortFileName   // Exclusive with LongFileName
    LongFileName
    Line            // Exclusive with FullLoc
    FullLoc

    Default = Prefix | Date | Time | LongFileName | FullLoc
}

interface ILogWriter
{
    mtd print(text: string)
}

struct Log
{
    lock:       Sync.Mutex
    buf:        StrConv.StringBuilder
    dt:         Time.DateTime
    writers:    Array'ILogWriter
    prefix:     string
    flags:      LogFlags = Default
}

impl Log
{
    internal mtd buildHeader(loc: Swag.SourceCodeLocation)
    {
        if flags & .Prefix and @countof(prefix)
        {
            buf.appendString(prefix)
            buf.appendString(" ")
        }

        if flags & .Date
        {
            buf.appendFormat("%/%{D02}/%{D02} ", dt.year, dt.month, dt.day)
        }

        if flags & .Time
        {
            buf.appendFormat("%{D02}:%{D02}:%{D02}:%{D03} ", dt.hour, dt.minute, dt.second, dt.millisecond)
        }

        if flags & .ShortFileName
        {
            buf.appendString(Path.getFileName(loc.fileName))
            buf.appendString(flags & (.Line|.FullLoc) ? ":" : " ")
        }
        elif flags & .LongFileName
        {
            buf.appendString(loc.fileName)
            buf.appendString(flags & (.Line|.FullLoc) ? ":" : " ")
        }

        if flags & .Line
        {
            buf.appendAny(loc.lineStart + 1)
            buf.appendString(" ")
        }
        elif flags & .FullLoc
        {
            buf.appendFormat("%:%:%:% ", loc.lineStart + 1, loc.colStart + 1, loc.lineEnd + 1, loc.colEnd + 1)
        }
    }

    // Create a new log
    func create()->Log
    {
        var result: retval
        return result
    }

    // Set the logger prefix
    mtd setPrefix(prefix: string)
    {
        Sync.scopedLock(&lock)
        self.prefix = #self.prefix
    }

    // Get the logger prefix
    mtd getPrefix()->string
    {
        Sync.scopedLock(&lock)
        return prefix
    }

    // Set the logger flags
    mtd setFlags(flags: LogFlags)
    {
        Sync.scopedLock(&lock)
        self.flags = #self.flags
    }

    // Get the logger prefix
    mtd getFlags()->LogFlags
    {
        Sync.scopedLock(&lock)
        return flags
    }

    // Register a new writer interface
    mtd addWriter(writer: ILogWriter)
    {
        Sync.scopedLock(&lock)
        writers.add(writer)
    }

    // Remove all writers
    mtd clearWriters()
    {
        Sync.scopedLock(&lock)
        writers.clear()
    }

    // Main print function
    mtd print(text: string, loc = #callerlocation)
    {
        // Get this first, to have the time of the call, before the lock
        if flags & (.Date|.Time) dt = Time.DateTime.now()

        Sync.scopedLock(&lock)

        // Build log string
        buf.clear()
        buildHeader(loc)
        buf.appendString(text)

        // Output default result if no writers
        if writers.isEmpty()
        {
            Console.lock()
            Console.println(buf.toString())
            Console.unlock()
            return
        }

        // Otherwise call all writers
        var str = buf.toString()
        visit w: writers
        {
            w.print(str)
        }
    }
}
