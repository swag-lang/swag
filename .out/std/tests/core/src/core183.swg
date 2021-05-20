
using Core

var G = 0's64
const N = 2
const ITER = 10000
var mutex: Sync.Mutex

func entry(th: Threading.Thread)
{
    mutex.lock()
    loop ITER G += 1
    mutex.unlock()
}

//#[Swag.printbc]
#test
{
    var th: [N] Threading.Thread
    visit *t: th catch t.init(&entry)
    visit *t: th catch t.start()
    visit *t: th t.wait()
    @assert(G == N * ITER)
}
