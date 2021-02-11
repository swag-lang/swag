#import "core"
using core

func entry(th: thread.Thread)
{
}

//#[swag.printbc]
#test
{
    var th: thread.Thread
    catch th.init(&entry)
    catch th.start()
    th.wait()
}
