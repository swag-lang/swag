#import "core"
using core.time

var G = 0

func entry(t: Timer)
{
    G += 1
}

//#[swag.printbc]
#test
{
    var timer: Timer
    timer.init(10, &entry, true) catch {}
    @assert(!@err)
    defer timer.release()
    thread.sleep(31)
    //@assert(G >= 1)
}