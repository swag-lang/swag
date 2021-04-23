#global public
using time

#[swag.enumflags]
enum LogFlags
{
    Date
    Time
    ShortFileName   // Exclusive with LongFileName
    LongFileName
    Line            // Exclusive with FullLoc
    FullLoc

    None    = 0
    Default = Date | Time | LongFileName | FullLoc
}

interface ILogWriter
{
    print: func(self, string)
}

struct Log
{
    lock:       sync.Mutex
    buf:        strconv.StringBuilder
    flags:      LogFlags = Default
    dt:         DateTime
    writers:    Array'ILogWriter
    prefix:     string
}

impl Log
{
    private func buildHeader(using self, loc: swag.CompilerSourceLocation)
    {
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
            buf.appendString(path.getFileName(loc.fileName))
            buf.appendString(flags & (.Line|.FullLoc) ? ":" : " ")
        }
        else if flags & .LongFileName
        {
            buf.appendString(loc.fileName)
            buf.appendString(flags & (.Line|.FullLoc) ? ":" : " ")
        }

        if flags & .Line
        {
            buf.appendAny(loc.lineStart + 1)
            buf.appendString(" ")
        }
        else if flags & .FullLoc
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
    func setPrefix(using self, prefix: string)
    {
        sync.scopedLock(lock)
        self.prefix = #scopefct.prefix
    }

    // Set the logger prefix
    func getPrefix(using self)->string
    {
        sync.scopedLock(lock)
        return prefix
    }

    // Set the logger flags
    func setFlags(using self, flags: LogFlags)
    {
        sync.scopedLock(lock)
        self.flags = #scopefct.flags
    }

    // Set the logger prefix
    func getFlags(using self)->LogFlags
    {
        sync.scopedLock(lock)
        return flags
    }

    // Register a new writer interface
    func addWriter(using self, writer: ILogWriter)
    {
        sync.scopedLock(lock)
        writers.add(writer)
    }

    // Remove all writers
    func clearWriters(using self)
    {
        sync.scopedLock(lock)
        writers.clear()
    }

    // Main print function
    func print(using self, text: string, loc = #callerlocation)
    {
        // Get this first, to have the time of the call, before the lock
        if flags & (.Date|.Time) dt = DateTime.now()

        sync.scopedLock(lock)

        // Build log string
        buf.clear()
        buildHeader(loc)
        buf.appendString(text)

        // Output default result if no writers
        if writers.isEmpty()
        {
            console.lock()
            console.println(buf.toString())
            console.unlock()
            return
        }

        // Otherwise call all writers
        visit w: writers
        {
            w.print(buf.toString())
        }
    }
}
