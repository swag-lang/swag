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
        if .flags & .Prefix and @countof(.prefix)
        {
            .buf.appendString(.prefix)
            .buf.appendString(" ")
        }

        if .flags & .Date
        {
            .buf.appendFormat("%/%{D02}/%{D02} ", .dt.year, .dt.month, .dt.day)
        }

        if .flags & .Time
        {
            .buf.appendFormat("%{D02}:%{D02}:%{D02}:%{D03} ", .dt.hour, .dt.minute, .dt.second, .dt.millisecond)
        }

        if .flags & .ShortFileName
        {
            .buf.appendString(Path.getFileName(loc.fileName))
            .buf.appendString(.flags & (.Line | .FullLoc) ? ":" : " ")
        }
        elif .flags & .LongFileName
        {
            .buf.appendString(loc.fileName)
            .buf.appendString(.flags & (.Line | .FullLoc) ? ":" : " ")
        }

        if .flags & .Line
        {
            .buf.appendAny(loc.lineStart + 1)
            .buf.appendString(" ")
        }
        elif .flags & .FullLoc
        {
            .buf.appendFormat("%:%:%:% ", loc.lineStart + 1, loc.colStart + 1, loc.lineEnd + 1, loc.colEnd + 1)
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
        Sync.scopedLock(&.lock)
        .prefix = prefix
    }

    // Get the logger prefix.
    mtd getPrefix()->string
    {
        Sync.scopedLock(&.lock)
        return .prefix
    }

    // Set the logger flags.
    mtd setFlags(flags: LogFlags)
    {
        Sync.scopedLock(&.lock)
        .flags = flags
    }

    // Get the logger flags.
    mtd getFlags()->LogFlags
    {
        Sync.scopedLock(&.lock)
        return .flags
    }

    // Register a new writer interface.
    mtd addWriter(writer: ILogWriter)
    {
        Sync.scopedLock(&.lock)
        .writers.add(writer)
    }

    // Remove all registered writers.
    mtd clearWriters()
    {
        Sync.scopedLock(&.lock)
        .writers.clear()
    }

    // Print a message to the log with an optional source location.
    mtd print(text: string, loc = #callerlocation)
    {
        // Get this first, to have the time of the call, before the lock
        if .flags & (.Date | .Time) do
            .dt = Time.DateTime.now()

        Sync.scopedLock(&.lock)

        // Build log string
        .buf.clear()
        .buildHeader(loc)
        .buf.appendString(text)

        // Output default result if no writers
        if .writers.isEmpty()
        {
            Console.lock()
            Console.println(.buf.toString())
            Console.unlock()
            return
        }

        // Otherwise call all writers
        let str = .buf.toString()
        foreach w in .writers
        {
            w.print(str)
        }
    }
}
