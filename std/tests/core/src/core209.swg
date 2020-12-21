#import "core"
using core

var G = 0's64
const N = 100

func entry(th: thread.Thread)
{
    var x: String = "this is a very long string"
}

#test
{
    //var th: [N] thread.Thread

    /*visit *t: th thread.create(t, &entry)
    visit *t: th t.start()
    thread.wait(&th[0], &th[1], &th[2])*/
}
