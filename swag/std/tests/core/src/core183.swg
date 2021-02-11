#import "core"
using core

var G = 0's64
const N = 2
const ITER = 10000
var mutex: sync.Mutex

func entry(th: thread.Thread)
{
    mutex.lock()
    loop ITER G += 1
    mutex.unlock()
}

//#[swag.printbc]
#test
{
    var th: [N] thread.Thread

    mutex.init() catch {}
    @assert(!@err)
    defer mutex.release()
    @assert(mutex.isValid())

    visit *t: th t.init(&entry) catch {}
    visit *t: th t.start() catch {}
    visit *t: th t.wait()
    @assert(G == N * ITER)
}
