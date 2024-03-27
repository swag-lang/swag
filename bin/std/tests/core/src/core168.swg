using Core.Time

//#[Swag.PrintBc]
#test
{
    var dt  = DateTime.now()
    var ts  = TimeSpan.from(dt)
    var dt1 = ts.toDateTime()
    /*Console.print("ticks: %\n", ts)
    Console.print("dt: %\n", dt)
    Console.print("dt1: %\n", dt1)*/
    @assert(dt1 == dt)
}
