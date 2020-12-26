#import "core"
using core.thread
alias createThread = thread.Thread.init

var G = 0's64
const ITER = 50000

func entry(th: Thread)
{
    loop ITER
    {
        atomic.add(&G, 1)
    }
}

//#[swag.printbc]
#test
{
    var th: [3] Thread
    createThread(&th[0], &entry)
    createThread(&th[1], &entry)
    createThread(&th[2], &entry)
    visit *t: th t.start()
    visit *t: th t.wait()
    @assert(G == 3 * ITER)
}
