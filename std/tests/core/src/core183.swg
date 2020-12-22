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

    mutex = sync.Mutex.create()
    defer mutex.release()
    @assert(mutex.isValid())

    visit *t: th thread.create(t, &entry)
    visit *t: th t.start()
    visit *t: th t.wait()
    @assert(G == N * ITER)
}
