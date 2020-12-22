#import "core"
using core.time

#test
{
    var timer: Timer
    @print("set\n")
    @assert(timer.init(true, 1000, null) == null)

    //now := nowMilliseconds()
    //while nowMilliseconds() - now < 2000 {}

    timer.release()
}