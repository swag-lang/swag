using Core.Time

//#[Swag.PrintBc]
#test
{
    let dt  = DateTime.now()
    let ts  = TimeSpan.from(dt)
    let dt1 = ts.toDateTime()
    /*Console.print("ticks: %\n", ts)
    Console.print("dt: %\n", dt)
    Console.print("dt1: %\n", dt1)*/
    @assert(dt1 == dt)
}
