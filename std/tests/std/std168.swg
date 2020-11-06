#import "std"
using std.time

//#[swag.printbc]
#test
{
    var dt: DateTime
    dt.year = 2020
    dt.month = 11
    dt.day = 6
    dt.hour = 15
    dt.minute = 48
    dt.second = 30
    dt.millisecond = 666

    var ts: TimeSpan
    ts.from(dt)
    dt1 := ts.getDateTime()
    console.print("year: %\n", dt1.year)
    console.print("month: %\n", dt1.month)
    console.print("day: %\n", dt1.day)
    console.print("hour: %\n", dt1.hour)
    console.print("minute: %\n", dt1.minute)
    console.print("second: %\n", dt1.second)
    console.print("millisecond: %\n", dt1.millisecond)

}