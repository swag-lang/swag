public namespace time;

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
    func opEquals(self, other: Self)->bool
    {
        return memory.compare(self, other, cast(uint) @sizeof(Self)) == 0
    }

    // Returns true if this is a valid datatime
    func isValid(using self)->bool
    {
        if year == 0 || year > 9999 return false
        if month == 0 || month > 12 return false
        if day == 0 || day > daysInMonth(year, month) return false
        if hour >= 24 return false
        if minute >= 60 return false
        if second >= 60 return false
        if millisecond >= 1000 return false
        return true
    }

    // Returns the day of week of the current date
    #[swag.property]
    func dayOfWeek(using self)->DayOfWeek
    {
        m := (month + 9) % 12
        y := year - m/10
        dn := 365*y + y/4 + y/400 - y/100 + (m*306 + 5)/10 + day-1
        dn += 3
        return cast(DayOfWeek) (dn % 7)
    }
}
