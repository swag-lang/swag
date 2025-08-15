#global marked
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
        if self.flags & .Prefix and @countof(self.prefix)
        {
            self.buf.appendString(self.prefix)
            self.buf.appendString(" ")
        }

        if self.flags & .Date
        {
            self.buf.appendFormat("%/%{D02}/%{D02} ", self.dt.year, self.dt.month, self.dt.day)
        }

        if self.flags & .Time
        {
            self.buf.appendFormat("%{D02}:%{D02}:%{D02}:%{D03} ", self.dt.hour, self.dt.minute, self.dt.second, self.dt.millisecond)
        }

        if self.flags & .ShortFileName
        {
            self.buf.appendString(Path.getFileName(loc.fileName))
            self.buf.appendString(self.flags & (.Line | .FullLoc) ? ":" : " ")
        }
        elif self.flags & .LongFileName
        {
            self.buf.appendString(loc.fileName)
            self.buf.appendString(self.flags & (.Line | .FullLoc) ? ":" : " ")
        }

        if self.flags & .Line
        {
            self.buf.appendAny(loc.lineStart + 1)
            self.buf.appendString(" ")
        }
        elif self.flags & .FullLoc
        {
            self.buf.appendFormat("%:%:%:% ", loc.lineStart + 1, loc.colStart + 1, loc.lineEnd + 1, loc.colEnd + 1)
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
        Sync.scopedLock(&self.lock)
        self.prefix = #self.prefix
    }

    // Get the logger prefix.
    mtd getPrefix()->string
    {
        Sync.scopedLock(&self.lock)
        return self.prefix
    }

    // Set the logger flags.
    mtd setFlags(flags: LogFlags)
    {
        Sync.scopedLock(&self.lock)
        self.flags = #self.flags
    }

    // Get the logger flags.
    mtd getFlags()->LogFlags
    {
        Sync.scopedLock(&self.lock)
        return self.flags
    }

    // Register a new writer interface.
    mtd addWriter(writer: ILogWriter)
    {
        Sync.scopedLock(&self.lock)
        self.writers.add(writer)
    }

    // Remove all registered writers.
    mtd clearWriters()
    {
        Sync.scopedLock(&self.lock)
        self.writers.clear()
    }

    // Print a message to the log with an optional source location.
    mtd print(text: string, loc = #callerlocation)
    {
        // Get this first, to have the time of the call, before the lock
        if self.flags & (.Date | .Time):
            self.dt = Time.DateTime.now()

        Sync.scopedLock(&self.lock)

        // Build log string
        self.buf.clear()
        self.buildHeader(loc)
        self.buf.appendString(text)

        // Output default result if no writers
        if self.writers.isEmpty()
        {
            Console.lock()
            Console.println(self.buf.toString())
            Console.unlock()
            return
        }

        // Otherwise call all writers
        var str = self.buf.toString()
        foreach w in self.writers
        {
            w.print(str)
        }
    }
}
