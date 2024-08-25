using Core

var G      = 0's64
const N    = 3
const ITER = 10000
var mutex: Sync.Mutex

func entry(th: Threading.Thread)
{
    mutex.lock()
    for ITER:
        G += 1
    mutex.unlock()
}

//#[Swag.PrintBc]
#test
{
    var th: [N] Threading.Thread
    foreach &t in th:
        catch t.init(&entry)
    foreach &t in th:
        catch t.start()
    Threading.wait(&th[0], &th[1], &th[2])
    @assert(G == N * ITER)
}
