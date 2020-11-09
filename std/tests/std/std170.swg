#import "std"
using std.time

#test
{
    {
        var x: DateTime{2020, 11, 8}
        y := x.dayOfWeek()
        console.print("%\n", y)
    }
    {
        var x: DateTime{2019, 7, 3}
        y := x.dayOfWeek()
        console.print("%\n", y)
    }
}