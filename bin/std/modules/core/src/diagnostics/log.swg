#global public

// Flags to control the format of log output.
#[Swag.EnumFlags]
enum LogFlags
{
    Zero          = 0                                                 // No flags set.
    Prefix                                                            // Include custom prefix in log output.
    Date                                                              // Include date (YYYY/MM/DD).
    Time                                                              // Include time (HH:MM:SS:ms).
    ShortFileName                                                     // Include only the file name (exclusive with LongFileName).
    LongFileName                                                      // Include full file path (exclusive with ShortFileName).
    Line                                                              // Include only the start line number (exclusive with FullLoc).
    FullLoc                                                           // Include full location (line/col start and end).
    Default       = Prefix | Date | Time | LongFileName | FullLoc     // Default logging configuration.
}

// Interface for writing log output.
interface ILogWriter
{
    mtd print(text: string); // Print a string to the log destination.
}

// A thread-safe logger that supports multiple output writers and customizable formatting.
struct Log
{
    lock:        Sync.Mutex                // Lock for thread-safe access.
    buf:         StrConv.StringBuilder     // Internal buffer for building log messages.
    dt:          Time.DateTime             // Cached date/time at the moment of log call.
    writers:     Array'ILogWriter          // Registered writer interfaces.
    prefix:      string                    // Optional prefix added to each log line.
    flags:       LogFlags = Default        // Bitwise flags controlling output format.
}

impl Log
{
    // Build the formatted log message header based on the current flags.
    internal mtd buildHeader(loc: Swag.SourceCodeLocation)
    {
        if me.flags & .Prefix and @countof(me.prefix)
        {
            me.buf.appendString(me.prefix)
            me.buf.appendString(" ")
        }

        if me.flags & .Date
        {
            me.buf.appendFormat("%/%{D02}/%{D02} ", me.dt.year, me.dt.month, me.dt.day)
        }

        if me.flags & .Time
        {
            me.buf.appendFormat("%{D02}:%{D02}:%{D02}:%{D03} ", me.dt.hour, me.dt.minute, me.dt.second, me.dt.millisecond)
        }

        if me.flags & .ShortFileName
        {
            me.buf.appendString(Path.getFileName(loc.fileName))
            me.buf.appendString(me.flags & (.Line | .FullLoc) ? ":" : " ")
        }
        elif me.flags & .LongFileName
        {
            me.buf.appendString(loc.fileName)
            me.buf.appendString(me.flags & (.Line | .FullLoc) ? ":" : " ")
        }

        if me.flags & .Line
        {
            me.buf.appendAny(loc.lineStart + 1)
            me.buf.appendString(" ")
        }
        elif me.flags & .FullLoc
        {
            me.buf.appendFormat("%:%:%:% ", loc.lineStart + 1, loc.colStart + 1, loc.lineEnd + 1, loc.colEnd + 1)
        }
    }

    // Create a new log instance.
    func create()->Log
    {
        var result: retval
        return result
    }

    // Set the logger prefix.
    mtd setPrefix(prefix: string)
    {
        Sync.scopedLock(&me.lock)
        me.prefix = #me.prefix
    }

    // Get the logger prefix.
    mtd getPrefix()->string
    {
        Sync.scopedLock(&me.lock)
        return me.prefix
    }

    // Set the logger flags.
    mtd setFlags(flags: LogFlags)
    {
        Sync.scopedLock(&me.lock)
        me.flags = #me.flags
    }

    // Get the logger flags.
    mtd getFlags()->LogFlags
    {
        Sync.scopedLock(&me.lock)
        return me.flags
    }

    // Register a new writer interface.
    mtd addWriter(writer: ILogWriter)
    {
        Sync.scopedLock(&me.lock)
        me.writers.add(writer)
    }

    // Remove all registered writers.
    mtd clearWriters()
    {
        Sync.scopedLock(&me.lock)
        me.writers.clear()
    }

    // Print a message to the log with an optional source location.
    mtd print(text: string, loc = #callerlocation)
    {
        // Get this first, to have the time of the call, before the lock
        if me.flags & (.Date | .Time):
            me.dt = Time.DateTime.now()

        Sync.scopedLock(&me.lock)

        // Build log string
        me.buf.clear()
        me.buildHeader(loc)
        me.buf.appendString(text)

        // Output default result if no writers
        if me.writers.isEmpty()
        {
            Console.lock()
            Console.println(me.buf.toString())
            Console.unlock()
            return
        }

        // Otherwise call all writers
        var str = me.buf.toString()
        foreach w in me.writers
        {
            w.print(str)
        }
    }
}
