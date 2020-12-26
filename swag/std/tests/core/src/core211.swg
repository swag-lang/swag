#import "core"
using core.time

var G = 0

func entry(t: Timer)
{
    G += 1
}

#test
{
    var timer: Timer
    @assert(timer.init(10, &entry, true) == null)
    defer timer.release()
    thread.sleep(30)
    @assert(G >= 1)
}