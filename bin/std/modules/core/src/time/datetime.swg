#global public
#global namespace Time

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
    Iso             // YYYY-MM-DD YYYY HH:MM:SS
    IsoMs           // YYYY-MM-DD YYYY HH:MM:SS.ZZZ
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
    mtdc opEquals(other: Self)->bool
    {
        return Memory.compare(self, other, cast(uint) @sizeof(Self)) == 0
    }

    mtdc opCmp(other: DateTime)->s32
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
    mtdc isValid()->bool
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
    mtdc dayOfWeek()->DayOfWeek
    {
        m := (month + 9) % 12
        y := year - m/10
        dn := 365*y +,safe y/4 +,safe y/400 +,safe (m*306 + 5)/10 +,safe day-1 -,safe y/100
        dn += 3
        return cast(DayOfWeek) (dn % 7)
    }

    // Returns the month name
    func monthName(month: u16)->string
    {
        Debug.assert(month >= 1 and month <= 12)
        const Names = @["", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"]
        return Names[month]
    }

    // Returns the day of week name
    func dayOfWeekName(day: DayOfWeek)->string
    {
        const Names = @["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"]
        return Names[cast(u32) day]
    }

    // Convert to string
    mtd toString(fmt = DateTimeFormat.DateTime)->String
    {
        #[Swag.Complete]
        switch fmt
        {
        case DateTime:
            return Format.toString(
                "% % % % %:%:%",
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
                "% % % % %:%:%.%",
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
    mtd convert(buf: *ConcatBuffer, fmt: const *StrConv.ConvertFormat, strFormat: string)
    {
        var str: String
        if @countof(strFormat) == 0 or @countof(strFormat) > 1
            str = toString(DateTimeFormat.DateTime)
        else
        {
            switch strFormat[0]
            {
            case "a"'u8:
                str = toString(DateTimeFormat.DateTime)
            case "b"'u8:
                str = toString(DateTimeFormat.DateTimeMs)
            case "c"'u8:
                str = toString(DateTimeFormat.Iso)
            case "d"'u8:
                str = toString(DateTimeFormat.IsoMs)
            }
        }

        buf.addBytes(str)
    }
}