using Core.Threading
namealias createThread = Threading.Thread.init

var G      = 0's64
const N    = 5
const ITER = 50000

func entry(th: Thread)
{
    for ITER
    {
        Atomic.add(&G, 1)
    }
}

//#[Swag.PrintBc]
#test
{
    var th: [N] Thread
    visit &t in th:
        catch createThread(t, &entry)
    visit &t in th:
        catch t.start()
    visit &t in th:
        t.wait()
    @assert(G == N * ITER)
}
