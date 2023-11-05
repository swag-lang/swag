#global public
#global namespace Time
using Errors

const MonthNames     = ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"]
const DayOfWeekNames = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"]

enum DayOfWeek
{
    Sunday = 0
    Monday = 1
    Tuesday = 2
    Wednesday = 3
    Thursday = 4
    Friday = 5
    Saturday = 6
}

enum DateTimeFormat
{
    DateTime        // DDDD MMMM D YYYY HH:MM:SS
    DateTimeMs      // DDDD MMMM D YYYY HH:MM:SS.ZZZ
    Iso             // YYYY-MM-DD HH:MM:SS
    IsoMs           // YYYY-MM-DD HH:MM:SS.ZZZ
}

// Represents an instant in time, typically expressed as a date and time of day
struct DateTime
{
    year:			u16
    month:			u16
    day:			u16
    hour:			u16
    minute:			u16
    second:			u16
    millisecond:	u16
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
        if year < other.year return -1
        if year > other.year return 1
        if month < other.month return -1
        if month > other.month return 1
        if day < other.day return -1
        if day > other.day return 1
        if hour < other.hour return -1
        if hour > other.hour return 1
        if minute < other.minute return -1
        if minute > other.minute return 1
        if second < other.second return -1
        if second > other.second return 1
        if millisecond < other.millisecond return -1
        if millisecond > other.millisecond return 1
        return 0
    }

    // Returns true if this is a valid datatime
    mtd const isValid()->bool
    {
        if year == 0 or year > 9999 return false
        if month == 0 or month > 12 return false
        if day == 0 or day > daysInMonth(year, month) return false
        if hour >= 24 return false
        if minute >= 60 return false
        if second >= 60 return false
        if millisecond >= 1000 return false
        return true
    }

    // Returns the day of week of the current date
    mtd const dayOfWeek()->DayOfWeek
    {
        let m: u32 = (month + 9) % 12
        let y: u32 = year - m/10
        var dn = 365*y +,over y/4 +,over y/400 +,over (m*306 + 5)/10 +,over day-1 -,over y/100
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

    // Convert a string to a DateTime with the given format
    func parse(str: string, fmt = DateTimeFormat.DateTime, checkDate = true)->{value: DateTime, eat: u32} throw
    {
        var result: retval

        var buf = str
        if !@countof(buf)
            throw SyntaxError{}

        if fmt == .DateTime or fmt == .DateTimeMs
        {
            // Day of week name
            var done = false
            var dayOfWeek: DayOfWeek
            visit dow, idx: DayOfWeekNames
            {
                let len = @countof(dow)
                if @mkstring(@dataof(buf), Math.min(@countof(buf), len)) == dow
                {
                    dayOfWeek = acast idx
                    buf = Tokenize.eatCount(buf, cast(u64) len)
                    result.eat += cast(u32) len
                    done = true
                    break
                }
            }

            if !done
                throw SyntaxError{}
            done = false

            // Blank
            if !@countof(buf) or buf[0] != ` `
                throw SyntaxError{}
            buf = Tokenize.eatCount(buf, 1)
            result.eat += 1

            // Month name
            visit dow, idx: MonthNames
            {
                let len = @countof(dow)
                if @mkstring(@dataof(buf), Math.min(@countof(buf), len)) == dow
                {
                    result.value.month = cast(u16) idx + 1
                    buf = Tokenize.eatCount(buf, cast(u64) len)
                    result.eat += cast(u32) len
                    done = true
                    break
                }
            }

            if !done
                throw SyntaxError{}

            // Blank
            if !@countof(buf) or buf[0] != ` `
                throw SyntaxError{}
            buf = Tokenize.eatCount(buf, 1)
            result.eat += 1

            // Day
            let (day, eat2, ovf2) = StrConv.parseU8(buf)
            if ovf2
                throw SyntaxError{}
            result.eat += eat2
            result.value.day = day
            buf = Tokenize.eatCount(buf, cast(u64) eat2)

            // Blank
            if !@countof(buf) or buf[0] != ` `
                throw SyntaxError{}
            buf = Tokenize.eatCount(buf, 1)
            result.eat += 1

            // Year
            let (year, eat3, ovf3) = StrConv.parseU16(buf)
            if ovf3
                throw SyntaxError{}
            result.value.year = year
            result.eat += eat3
            buf = Tokenize.eatCount(buf, cast(u64) eat3)

            // Check day of week
            if checkDate and dayOfWeek != result.value.dayOfWeek()
                throw SyntaxError{}
        }
        elif fmt == .Iso or fmt == .IsoMs
        {
            // Year
            let (year, eat0, ovf0) = StrConv.parseU16(buf)
            if ovf0
                throw SyntaxError{}
            result.value.year = year
            result.eat += eat0
            buf = Tokenize.eatCount(buf, cast(u64) eat0)
            if !@countof(buf) or buf[0] != `-`
                throw SyntaxError{}
            buf = Tokenize.eatCount(buf, 1)
            result.eat += 1

            // Month
            let (month, eat1, ovf1) = StrConv.parseU8(buf)
            if ovf1
                throw SyntaxError{}
            result.eat += eat1
            result.value.month = month
            buf = Tokenize.eatCount(buf, cast(u64) eat1)
            if !@countof(buf) or buf[0] != `-`
                throw SyntaxError{}
            buf = Tokenize.eatCount(buf, 1)
            result.eat += 1

            // Day
            let (day, eat2, ovf2) = StrConv.parseU8(buf)
            if ovf2
                throw SyntaxError{}
            result.eat += eat2
            result.value.day = day
            buf = Tokenize.eatCount(buf, cast(u64) eat2)
        }

        // Blank
        if !@countof(buf) or buf[0] != ` `
            throw SyntaxError{}
        buf = Tokenize.eatCount(buf, 1)
        result.eat += 1

        // Hour
        let (hour, eat3, ovf3) = StrConv.parseU8(buf)
        if ovf3
            throw SyntaxError{}
        result.eat += eat3
        result.value.hour = hour
        buf = Tokenize.eatCount(buf, cast(u64) eat3)
        if !@countof(buf) or buf[0] != `:`
            throw SyntaxError{}
        buf = Tokenize.eatCount(buf, 1)
        result.eat += 1

        // Minute
        let (minute, eat4, ovf4) = StrConv.parseU8(buf)
        if ovf4
            throw SyntaxError{}
        result.eat += eat4
        result.value.minute = minute
        buf = Tokenize.eatCount(buf, cast(u64) eat4)
        if !@countof(buf) or buf[0] != `:`
            throw SyntaxError{}
        buf = Tokenize.eatCount(buf, 1)
        result.eat += 1

        // Seconds
        let (second, eat5, ovf5) = StrConv.parseU8(buf)
        if ovf5
            throw SyntaxError{}
        result.eat += eat5
        result.value.second = second
        buf = Tokenize.eatCount(buf, cast(u64) eat5)

        // Milliseconds
        if fmt == .DateTimeMs or fmt == .IsoMs
        {
            if !@countof(buf) or buf[0] != `.`
                throw SyntaxError{}
            buf = Tokenize.eatCount(buf, 1)
            result.eat += 1

            let (millisecond, eat6, ovf6) = StrConv.parseU16(buf)
            if ovf6
                throw SyntaxError{}
            result.eat += eat6
            result.value.millisecond = millisecond
            buf = Tokenize.eatCount(buf, cast(u64) eat6)
        }

        if checkDate and !result.value.isValid()
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
            return Format.toString(
                "% % % % %{02}:%{02}:%{02}",
                dayOfWeekName(dayOfWeek()),
                monthName(month),
                day,
                year,
                hour,
                minute,
                second
            )
        case DateTimeMs:
            return Format.toString(
                "% % % % %{02}:%{02}:%{02}.%",
                dayOfWeekName(dayOfWeek()),
                monthName(month),
                day,
                year,
                hour,
                minute,
                second,
                millisecond
            )
        case Iso:
            return Format.toString(
                "%-%{02}-%{02} %{02}:%{02}:%{02}",
                year,
                month,
                day,
                hour,
                minute,
                second
            )
        case IsoMs:
            return Format.toString(
                "%-%{02}-%{02} %{02}:%{02}:%{02}.%",
                year,
                month,
                day,
                hour,
                minute,
                second,
                millisecond
            )
        }

        var result: retval
        return result
    }
}

impl StrConv.IConvert for DateTime
{
    mtd impl convert(buf: *ConcatBuffer, fmt: const *StrConv.ConvertFormat, strFormat: string)
    {
        var str: String
        if @countof(strFormat) == 0 or @countof(strFormat) > 1
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
                str = toString(.Iso)
            case `d`:
                str = toString(.IsoMs)
            }
        }

        buf.addBytes(str)
    }
}