
using core

var G = 0's64
const N = 3
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
    visit *t: th catch t.init(&entry)
    visit *t: th catch t.start()
    thread.wait(&th[0], &th[1], &th[2])
    @assert(G == N * ITER)
}
