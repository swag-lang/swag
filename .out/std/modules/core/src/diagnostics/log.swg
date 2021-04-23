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

    Default = Date | Time | LongFileName | FullLoc
}

struct Log
{
    lock:   sync.Mutex
    buf:    strconv.StringBuilder
    flags:  LogFlags = Default
    dt:     DateTime
}

impl Log
{
    private func buildHeader(using self, loc: swag.CompilerSourceLocation)
    {
        buf.clear()

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
        result.init()
        return result
    }

    // Initialize logger
    func init(using self)
    {
    }

    func print(using self, text: string, loc = #callerlocation)
    {
        // Get this first, to have the time of the call, before the lock
        if flags & (.Date|.Time) dt = DateTime.now()

        sync.scopedLock(lock)

        buildHeader(loc)
        buf.appendString(text)

        console.print(buf.toString())
    }
}