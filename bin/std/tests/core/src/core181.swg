using Core.Threading
alias createThread = Threading.Thread.init

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
    foreach &t in th:
        catch createThread(t, &entry)
    foreach &t in th:
        catch t.start()
    foreach &t in th:
        t.wait()
    @assert(G == N * ITER)
}
