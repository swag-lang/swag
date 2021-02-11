#import "core"
using core

func entry(th: thread.Thread)
{
}

//#[swag.printbc]
#test
{
    var th: thread.Thread
    th.init(&entry) catch {}
    th.start() catch {}
    th.wait()
}
