using core.Time

var G = 0

func entry(t: Timer)
{
    G += 1
}

//#[Swag.printbc]
#test
{
    var timer: Timer
    catch timer.init(10, &entry, true)
    @assert(!@err)
    defer timer.release()
    thread.sleep(31)
    //@assert(G >= 1)
}