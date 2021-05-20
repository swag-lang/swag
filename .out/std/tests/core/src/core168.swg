
using core.Time

//#[Swag.printbc]
#test
{
    dt := DateTime.now()
    ts := TimeSpan.from(dt)
    dt1 := ts.toDateTime()
    /*Console.print("ticks: %\n", ts)
    Console.print("dt: %\n", dt)
    Console.print("dt1: %\n", dt1)*/
    @assert(dt1 == dt)
}