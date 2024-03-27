
using Core.Threading
namealias createThread = Threading.Thread.init

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

//#[Swag.PrintBc]
#test
{
    var th: [N] Thread
    visit &t: th do catch createThread(t, &entry)
    visit &t: th do catch t.start()
    visit &t: th do t.wait()
    @assert(G == N * ITER)
}
