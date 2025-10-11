#global public
#global namespace Time
using Errors

// Names of the months used for formatting and parsing
const MonthNames = ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"]
// Names of the days of the week used for formatting and parsing
const DayOfWeekNames = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"]

// Day of the week enumeration
enum DayOfWeek
{
    Sunday    = 0     // Sunday
    Monday    = 1     // Monday
    Tuesday   = 2     // Tuesday
    Wednesday = 3     // Wednesday
    Thursday  = 4     // Thursday
    Friday    = 5     // Friday
    Saturday  = 6     // Saturday
}

// Supported formats for converting DateTime to/from string
enum DateTimeFormat
{
    DateIso           // 'YYYY-MM-DD'
    TimeIso           // 'HH:MM:SS'
    TimeIsoMs         // 'HH:MM:SS.ZZZ'
    TimeIsoHM         // 'HH:MM'
    Date              // 'DAYOFWEEK MONTH DAY YYYY'
    DateTime          // 'DAYOFWEEK MONTH DAY YYYY HH:MM:SS'
    DateTimeMs        // 'DAYOFWEEK MONTH DAY YYYY HH:MM:SS.ZZZ'
    DateTimeIso       // 'YYYY-MM-DD HH:MM:SS'
    DateTimeIsoMs     // 'YYYY-MM-DD HH:MM:SS.ZZZ'
    DateTimeIsoHM     // 'YYYY-MM-DD HH:MM'
}

// Represents an instant in time, typically expressed as a date and time of day
#[Serialization.Final]
struct DateTime
{
    year:            u16     // Year component (1 to 9999)
    month:           u16     // Month component (1 to 12)
    day:             u16     // Day component (1 to 31 depending on the month)
    hour:            u16     // Hour component (0 to 23)
    minute:          u16     // Minute component (0 to 59)
    second:          u16     // Second component (0 to 59)
    millisecond:     u16     // Millisecond component (0 to 999)
}

impl DateTime
{
    #[Swag.Inline]
    // Compares if two DateTime instances are equal
    mtd const opEquals(other: DateTime)->bool
    {
        return Memory.compare(me, other, cast(u64) #sizeof(DateTime)) == 0
    }

    // Compares two DateTime values. Returns -1, 0, or 1.
    mtd const opCmp(other: DateTime)->s32
    {
        if .year < other.year do
            return -1
        if .year > other.year do
            return 1
        if .month < other.month do
            return -1
        if .month > other.month do
            return 1
        if .day < other.day do
            return -1
        if .day > other.day do
            return 1
        if .hour < other.hour do
            return -1
        if .hour > other.hour do
            return 1
        if .minute < other.minute do
            return -1
        if .minute > other.minute do
            return 1
        if .second < other.second do
            return -1
        if .second > other.second do
            return 1
        if .millisecond < other.millisecond do
            return -1
        if .millisecond > other.millisecond do
            return 1
        return 0
    }

    // Returns true if this is a valid DateTime
    mtd const isValid()->bool
    {
        if .year == 0 or .year > 9999 do
            return false
        if .month == 0 or .month > 12 do
            return false
        if .day == 0 or .day > daysInMonth(.year, .month) do
            return false
        if .hour >= 24 do
            return false
        if .minute >= 60 do
            return false
        if .second >= 60 do
            return false
        if .millisecond >= 1000 do
            return false
        return true
    }

    // Returns the day of the week for the current date
    mtd const dayOfWeek()->DayOfWeek
    {
        let m: u32 = (.month + 9) % 12
        let y: u32 = .year - m / 10
        var dn = 365 * y + #wrap y / 4 + #wrap y / 400 + #wrap (m * 306 + 5) / 10 + #wrap .day - 1 - #wrap y / 100
        dn += 3
        return cast(DayOfWeek) (dn % 7)
    }

    // Returns the month name corresponding to a month number (1 to 12)
    func monthName(month: u16)->string
    {
        Debug.assert(month >= 1 and month <= 12)
        return MonthNames[month - 1]
    }

    // Returns the name of the given day of the week
    func dayOfWeekName(day: DayOfWeek)->string
    {
        return DayOfWeekNames[cast(u32) day]
    }

    // Parses an 8-bit unsigned integer from the buffer and updates the consumed character count
    private func parseU8(buf: &string, eat: &u32)->u16 throw
    {
        let (year, eat0, ovf) = StrConv.parseU8(buf)
        if ovf do
            throw SyntaxError{}
        eat += eat0
        buf = Tokenize.eatCount(buf, cast(u64) eat0)
        return year
    }

    // Parses a 16-bit unsigned integer from the buffer and updates the consumed character count
    private func parseU16(buf: &string, eat: &u32)->u16 throw
    {
        let (year, eat0, ovf) = StrConv.parseU16(buf)
        if ovf do
            throw SyntaxError{}
        eat += eat0
        buf = Tokenize.eatCount(buf, cast(u64) eat0)
        return year
    }

    // Parses a separator character from the buffer and updates the consumed character count
    private func parseSep(buf: &string, sep: u8, eat: &u32) throw
    {
        if !@countof(buf) or buf[0] != sep do
            throw SyntaxError{}
        buf = Tokenize.eatCount(buf, 1)
        eat += 1
    }

    // Converts a string to a DateTime using the expected format
    func parse(str: string, checkDate = true)->{ value: DateTime, eat: u32 } throw
    {
        var result: retval
        var buf     = str

        if !@countof(buf) do
            throw SyntaxError{}

        if @countof(buf) >= 1 and
           Latin1.isLetter(buf[0])
        {
            // Day of week name
            var done       = false
            var dayOfWeek: DayOfWeek
            foreach dow, idx in DayOfWeekNames
            {
                let len = @countof(dow)
                if @mkstring(@dataof(buf), Math.min(@countof(buf), len)) == dow
                {
                    dayOfWeek = cast() idx
                    buf       = Tokenize.eatCount(buf, cast(u64) len)
                    result.eat += cast(u32) len
                    done = true
                    break
                }
            }

            if !done do
                throw SyntaxError{}

            done = false
            parseSep(&buf, ' ', &result.eat)

            // Month name
            foreach dow, idx in MonthNames
            {
                let len = @countof(dow)
                if @mkstring(@dataof(buf), Math.min(@countof(buf), len)) == dow
                {
                    result.value.month = cast(u16) idx + 1
                    buf                = Tokenize.eatCount(buf, cast(u64) len)
                    result.eat += cast(u32) len
                    done = true
                    break
                }
            }

            if !done do
                throw SyntaxError{}

            parseSep(&buf, ' ', &result.eat)
            result.value.day = parseU8(&buf, &result.eat)
            parseSep(&buf, ' ', &result.eat)
            result.value.year = parseU16(&buf, &result.eat)

            // Check day of week
            if checkDate and dayOfWeek != result.value.dayOfWeek() do
                throw SyntaxError{}
        }
        elif @countof(buf) >= 10 and
             Latin1.isDigit(buf[0]) and
             Latin1.isDigit(buf[1]) and
             Latin1.isDigit(buf[2]) and
             Latin1.isDigit(buf[3]) and
             buf[4] == '-' and
             Latin1.isDigit(buf[5]) and
             Latin1.isDigit(buf[6]) and
             buf[7] == '-' and
             Latin1.isDigit(buf[8]) and
             Latin1.isDigit(buf[9])
        {
            result.value.year = parseU16(&buf, &result.eat)
            parseSep(&buf, '-', &result.eat)
            result.value.month = parseU8(&buf, &result.eat)
            parseSep(&buf, '-', &result.eat)
            result.value.day = parseU8(&buf, &result.eat)
        }
        else
        {
            throw SyntaxError{}
        }

        if @countof(buf) >= 1 and
           buf[0] == ' '
        {
            parseSep(&buf, ' ', &result.eat)
        }

        // Time
        if @countof(buf) >= 5 and
           Latin1.isDigit(buf[0]) and
           Latin1.isDigit(buf[1]) and
           buf[2] == ':' and
           Latin1.isDigit(buf[3]) and
           Latin1.isDigit(buf[4])
        {
            result.value.hour = parseU8(&buf, &result.eat)
            parseSep(&buf, ':', &result.eat)
            result.value.minute = parseU8(&buf, &result.eat)

            // Seconds
            if @countof(buf) >= 3 and
               buf[0] == ':' and
               Latin1.isDigit(buf[1]) and
               Latin1.isDigit(buf[2])
            {
                parseSep(&buf, ':', &result.eat)
                result.value.second = parseU8(&buf, &result.eat)
            }

            // Milliseconds
            if @countof(buf) >= 2 and
               buf[0] == '.' and
               Latin1.isDigit(buf[1])
            {
                parseSep(&buf, '.', &result.eat)
                result.value.millisecond = parseU16(&buf, &result.eat)
            }
        }

        if checkDate and !result.value.isValid() do
            throw SyntaxError{}

        return result
    }

    // Converts the DateTime to a string using the given format
    mtd const toString(fmt = DateTimeFormat.DateTime)->String
    {
        #[Swag.Complete]
        switch fmt
        {
        case DateIso:
            return Format.toString("%-%{02}-%{02}", .year, .month, .day)
        case TimeIso:
            return Format.toString("%{02}:%{02}:%{02}", .hour, .minute, .second)
        case TimeIsoMs:
            return Format.toString("%{02}:%{02}:%{02}.%", .hour, .minute, .second, .millisecond)
        case TimeIsoHM:
            return Format.toString("%{02}:%{02}", .hour, .minute)
        case Date:
            return Format.toString("% % % %", dayOfWeekName(.dayOfWeek()), monthName(.month), .day, .year)
        case DateTime:
            return Format.toString("% % % % %{02}:%{02}:%{02}", dayOfWeekName(.dayOfWeek()), monthName(.month), .day, .year, .hour, .minute, .second)
        case DateTimeMs:
            return Format.toString("% % % % %{02}:%{02}:%{02}.%", dayOfWeekName(.dayOfWeek()), monthName(.month), .day, .year, .hour, .minute, .second, .millisecond)
        case DateTimeIso:
            return Format.toString("%-%{02}-%{02} %{02}:%{02}:%{02}", .year, .month, .day, .hour, .minute, .second)
        case DateTimeIsoMs:
            return Format.toString("%-%{02}-%{02} %{02}:%{02}:%{02}.%", .year, .month, .day, .hour, .minute, .second, .millisecond)
        case DateTimeIsoHM:
            return Format.toString("%-%{02}-%{02} %{02}:%{02}", .year, .month, .day, .hour, .minute)
        }

        var result: retval
        return result
    }
}

impl Serialization.ISerialize for DateTime
{
    // Reads a DateTime from a textual encoder
    mtd impl read(decoder: Serialization.IDecoder)->bool throw
    {
        if decoder.isTextual()
        {
            let val = decoder.readString()
            dref me = parse(val).value
            return true
        }

        return false
    }

    // Writes a DateTime to a textual encoder
    mtd impl write(encoder: Serialization.IEncoder)->bool throw
    {
        if encoder.isTextual()
        {
            let val = .toString(.DateTimeIsoMs)
            encoder.writeString(val)
            return true
        }

        return false
    }

    mtd impl readElement(type: Swag.TypeValue, data: *void, decoder: Serialization.IDecoder)->bool throw
    {
        return false
    }

    mtd impl writeElement(type: Swag.TypeValue, data: const *void, decoder: Serialization.IEncoder)->bool throw
    {
        return false
    }

    mtd impl postRead(data: *void, decoder: Serialization.IDecoder) throw
    {
    }
}

impl StrConv.IConvert for DateTime
{
    // Converts a DateTime to a string using a single-character format specifier (for use with Format.toString)
    mtd impl convert(buf: *ConcatBuffer, convFormat: StrConv.ConvertFormat, strFormat: string)
    {
        var str: String
        if @countof(strFormat) == 0 or @countof(strFormat) > 1 do
            str = .toString(.DateTime)
        else
        {
            switch strFormat[0]
            {
            case 'a':
                str = .toString(.DateTime)
            case 'b':
                str = .toString(.DateTimeMs)
            case 'c':
                str = .toString(.DateTimeIso)
            case 'd':
                str = .toString(.DateTimeIsoMs)
            case 'e':
                str = .toString(.DateTimeIsoHM)
            case 'f':
                str = .toString(.DateIso)
            case 'g':
                str = .toString(.TimeIso)
            case 'h':
                str = .toString(.TimeIsoMs)
            case 'i':
                str = .toString(.TimeIsoHM)
            case 'j':
                str = .toString(.Date)
            default:
                str = .toString(.DateTime)
            }
        }

        buf.addBytes(str)
    }
}
