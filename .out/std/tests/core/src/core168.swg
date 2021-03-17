
using core.time

//#[swag.printbc]
#test
{
    dt := DateTime.now()
    ts := TimeSpan.from(dt)
    dt1 := ts.toDateTime()
    /*console.print("ticks: %\n", ts)
    console.print("dt: %\n", dt)
    console.print("dt1: %\n", dt1)*/
    @assert(dt1 == dt)
}