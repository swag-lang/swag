using Core

var G      = 0's64
const N    = 2
const ITER = 10000
var mutex: Sync.Mutex

func entry(th: Threading.Thread)
{
    mutex.lock()
    for ITER do
        G += 1
    mutex.unlock()
}

//#[Swag.PrintBc]
#test
{
    var th: [N] Threading.Thread
    foreach &t in th do
        catch t.init(&entry)
    foreach &t in th do
        catch t.start()
    foreach &t in th do
        t.wait()
    @assert(G == N * ITER)
}
