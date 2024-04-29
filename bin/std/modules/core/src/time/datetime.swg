#global public
#global namespace Time
using Errors

const MonthNames     = ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"]
const DayOfWeekNames = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"]

enum DayOfWeek
{
    Sunday    = 0
    Monday    = 1
    Tuesday   = 2
    Wednesday = 3
    Thursday  = 4
    Friday    = 5
    Saturday  = 6
}

enum DateTimeFormat
{
    DateTime       // `DAYOFWEEK MONTH DAY YYYY HH:MM:SS`
    DateTimeMs     // `DAYOFWEEK MONTH DAY YYYY HH:MM:SS.ZZZ`
    DateTimeIso    // `YYYY-MM-DD HH:MM:SS`
    DateTimeIsoMs  // `YYYY-MM-DD HH:MM:SS.ZZZ`
    DateTimeIsoHM  // `YYYY-MM-DD HH:MM`
}

// Represents an instant in time, typically expressed as a date and time of day
struct DateTime
{
    year:            u16
    month:           u16
    day:             u16
    hour:            u16
    minute:          u16
    second:          u16
    millisecond:     u16
}

impl DateTime
{
    #[Swag.Inline]
    mtd const opEquals(other: Self)->bool
    {
        return Memory.compare(self, other, cast(u64) @sizeof(Self)) == 0
    }

    mtd const opCmp(other: DateTime)->s32
    {
        if year < other.year do
            return -1
        if year > other.year do
            return 1
        if month < other.month do
            return -1
        if month > other.month do
            return 1
        if day < other.day do
            return -1
        if day > other.day do
            return 1
        if hour < other.hour do
            return -1
        if hour > other.hour do
            return 1
        if minute < other.minute do
            return -1
        if minute > other.minute do
            return 1
        if second < other.second do
            return -1
        if second > other.second do
            return 1
        if millisecond < other.millisecond do
            return -1
        if millisecond > other.millisecond do
            return 1
        return 0
    }

    // Returns true if this is a valid datatime
    mtd const isValid()->bool
    {
        if year == 0 or year > 9999 do
            return false
        if month == 0 or month > 12 do
            return false
        if day == 0 or day > daysInMonth(year, month) do
            return false
        if hour >= 24 do
            return false
        if minute >= 60 do
            return false
        if second >= 60 do
            return false
        if millisecond >= 1000 do
            return false
        return true
    }

    // Returns the day of week of the current date
    mtd const dayOfWeek()->DayOfWeek
    {
        let m: u32 = (month + 9) % 12
        let y: u32 = year - m / 10
        var dn = 365 * y +,over y / 4 +,over y / 400 +,over (m * 306 + 5) / 10 +,over day - 1 -,over y / 100
        dn += 3
        return cast(DayOfWeek) (dn % 7)
    }

    // Returns the month name
    func monthName(month: u16)->string
    {
        Debug.assert(month >= 1 and month <= 12)
        return MonthNames[month - 1]
    }

    // Returns the day of week name
    func dayOfWeekName(day: DayOfWeek)->string
    {
        return DayOfWeekNames[cast(u32) day]
    }

    private func parseU8(buf: &string, eat: &u32)->u16 throw
    {
        let (year, eat0, ovf) = StrConv.parseU8(buf)
        if ovf do
            throw SyntaxError{}
        eat += eat0
        buf = Tokenize.eatCount(buf, cast(u64) eat0)
        return year
    }

    private func parseU16(buf: &string, eat: &u32)->u16 throw
    {
        let (year, eat0, ovf) = StrConv.parseU16(buf)
        if ovf do
            throw SyntaxError{}
        eat += eat0
        buf = Tokenize.eatCount(buf, cast(u64) eat0)
        return year
    }

    private func parseSep(buf: &string, sep: u8, eat: &u32) throw
    {
        if !@countof(buf) or buf[0] != sep do
            throw SyntaxError{}
        buf = Tokenize.eatCount(buf, 1)
        eat += 1
    }

    // Convert a string to a DateTime with the given format
    func parse(str: string, fmt = DateTimeFormat.DateTime, checkDate = true)->{ value: DateTime, eat: u32 } throw
    {
        var result: retval

        var buf = str
        if !@countof(buf) do
            throw SyntaxError{}

        if fmt == .DateTime or fmt == .DateTimeMs
        {
            // Day of week name
            var done       = false
            var dayOfWeek: DayOfWeek
            visit dow, idx: DayOfWeekNames
            {
                let len = @countof(dow)
                if @mkstring(@dataof(buf), Math.min(@countof(buf), len)) == dow
                {
                    dayOfWeek = acast idx
                    buf       = Tokenize.eatCount(buf, cast(u64) len)
                    result.eat += cast(u32) len
                    done = true
                    break
                }
            }

            if !done do
                throw SyntaxError{}

            done = false
            parseSep(&buf, ` `, &result.eat)

            // Month name
            visit dow, idx: MonthNames
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

            parseSep(&buf, ` `, &result.eat)
            result.value.day = parseU8(&buf, &result.eat)
            parseSep(&buf, ` `, &result.eat)
            result.value.year = parseU16(&buf, &result.eat)

            // Check day of week
            if checkDate and dayOfWeek != result.value.dayOfWeek() do
                throw SyntaxError{}
        }
        elif fmt == .DateTimeIso or fmt == .DateTimeIsoMs or fmt == .DateTimeIsoHM
        {
            result.value.year = parseU16(&buf, &result.eat)
            parseSep(&buf, `-`, &result.eat)
            result.value.month = parseU8(&buf, &result.eat)
            parseSep(&buf, `-`, &result.eat)
            result.value.day = parseU8(&buf, &result.eat)
        }

        parseSep(&buf, ` `, &result.eat)
        result.value.hour = parseU8(&buf, &result.eat)
        parseSep(&buf, `:`, &result.eat)
        result.value.minute = parseU8(&buf, &result.eat)

        // Seconds
        if fmt != .DateTimeIsoHM
        {
            parseSep(&buf, `:`, &result.eat)
            result.value.second = parseU8(&buf, &result.eat)
        }

        // Milliseconds
        if fmt == .DateTimeMs or fmt == .DateTimeIsoMs
        {
            parseSep(&buf, `.`, &result.eat)
            result.value.millisecond = parseU16(&buf, &result.eat)
        }

        if checkDate and !result.value.isValid() do
            throw SyntaxError{}

        return result
    }

    // Convert to string
    mtd toString(fmt = DateTimeFormat.DateTime)->String
    {
        #[Swag.Complete]
        switch fmt
        {
        case DateTime:
            return Format.toString("% % % % %{02}:%{02}:%{02}", dayOfWeekName(dayOfWeek()), monthName(month), day, year, hour, minute, second)
        case DateTimeMs:
            return Format.toString("% % % % %{02}:%{02}:%{02}.%", dayOfWeekName(dayOfWeek()), monthName(month), day, year, hour, minute, second, millisecond)
        case DateTimeIso:
            return Format.toString("%-%{02}-%{02} %{02}:%{02}:%{02}", year, month, day, hour, minute, second)
        case DateTimeIsoMs:
            return Format.toString("%-%{02}-%{02} %{02}:%{02}:%{02}.%", year, month, day, hour, minute, second, millisecond)
        case DateTimeIsoHM:
            return Format.toString("%-%{02}-%{02} %{02}:%{02}", year, month, day, hour, minute)
        }

        var result: retval
        return result
    }
}

impl StrConv.IConvert for DateTime
{
    // Used when printing a date with [[Core.Format.toString]].
    // | 'a' | `DateTimeFormat.DateTime`
    // | 'b' | `DateTimeFormat.DateTimeMs`
    // | 'c' | `DateTimeFormat.DateTimeIso`
    // | 'd' | `DateTimeFormat.DateTimeIsoMs`
    // | 'e' | `DateTimeFormat.DateTimeIsoHM`
    //
    // ```swag
    // let myDate = DateTime.now()
    // Console.print(Format.toString("%{a}", myDate))
    // ```
    // See [[Core.Time.DateTimeFormat]]
    mtd impl convert(buf: *ConcatBuffer, convFormat: StrConv.ConvertFormat, strFormat: string)
    {
        var str: String
        if @countof(strFormat) == 0 or @countof(strFormat) > 1 do
            str = toString(.DateTime)
        else
        {
            switch strFormat[0]
            {
            case `a`:
                str = toString(.DateTime)
            case `b`:
                str = toString(.DateTimeMs)
            case `c`:
                str = toString(.DateTimeIso)
            case `d`:
                str = toString(.DateTimeIsoMs)
            case `e`:
                str = toString(.DateTimeIsoHM)
            }
        }

        buf.addBytes(str)
    }
}
