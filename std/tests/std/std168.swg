#import "std"
using std.time

//#[swag.printbc]
#test
{
    dt := DateTime.now()
    ts := TimeSpan.from(dt)
    dt1 := ts.getDateTime()
    @assert(dt1 == dt)
    //console.print("%\n", dt)
    //console.print("%\n", dt1)

}