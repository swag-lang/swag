
using Core.Threading
alias createThread = Threading.Thread.init

var G = 0's64
const N = 5
const ITER = 50000

func entry(th: Thread)
{
    loop ITER
    {
        Atomic.add(&G, 1)
    }
}

//#[Swag.printbc]
#test
{
    var th: [N] Thread
    visit *t: th catch createThread(t, &entry)
    visit *t: th catch t.start()
    visit *t: th t.wait()
    @assert(G == N * ITER)
}
