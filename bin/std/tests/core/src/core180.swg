using Core.Threading
alias createThread = Threading.Thread.init

var G      = 0's64
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
    var th: [3] Thread
    catch createThread(&th[0], &entry)
    catch createThread(&th[1], &entry)
    catch createThread(&th[2], &entry)
    foreach &t in th do
        catch t.start()
    foreach &t in th do
        t.wait()
    @assert(G == 3 * ITER)
}
