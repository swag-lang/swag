using core

#[swag.tls]
var G = 0's64

const N = 3
const ITER = 100000

func entry(th: thread.Thread)
{
    loop ITER G += 1
    @assert(G == ITER)
}

//#[swag.printbc]
#test
{
    var th: [N] thread.Thread
    visit *t: th catch t.init(&entry)
    visit *t: th catch t.start()
    thread.wait(&th[0], &th[1], &th[2])
}
