
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

//#[Swag.printbc]
#test
{
    var th: [3] Thread
    catch createThread(&th[0], &entry)
    catch createThread(&th[1], &entry)
    catch createThread(&th[2], &entry)
    visit *t: th catch t.start()
    visit *t: th t.wait()
    @assert(G == 3 * ITER)
}
