using Core

#[Swag.Tls]
var G = 0's64

const N = 3
const ITER = 100000

func entry(th: Threading.Thread)
{
    loop ITER G += 1
    @assert(G == ITER)
}

//#[Swag.PrintBc]
#test
{
    var th: [N] Threading.Thread
    visit *t: th catch t.init(&entry)
    visit *t: th catch t.start()
    Threading.wait(&th[0], &th[1], &th[2])
}
