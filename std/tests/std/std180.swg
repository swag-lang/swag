#import "std"
using std.thread

var G = 0's64
const ITER = 50000

func entry(th: thread.Thread)
{
    loop ITER
    {
        //G += 1
        atomic.add(&G, 1)
    }
}

//#[swag.printbc]
#test
{
    var th: [3] Thread
    thread.create(&th[0], &entry)
    thread.create(&th[1], &entry)
    thread.create(&th[2], &entry)
    visit *t: th t.start()
    visit *t: th t.wait()
    @assert(G == 3 * ITER)
}
