using Core.Time

var G = 0

func entry(t: Timer)
{
    G += 1
}

//#[Swag.PrintBc]
#test
{
    var timer: Timer
    catch timer.init(10, &entry, true)
    @assert(!@errmsg)
    Threading.Thread.sleep(30'ms)
    timer.release()
}