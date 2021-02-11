#import "core"
using core

var th1, th2: thread.Thread

func entry1(th: thread.Thread)
{
    //@print("entry1\n")
}

func entry2(th: thread.Thread)
{
    //@print("entry2\n")
    th1.wait()
    //@print("end entry2\n")
}

//#[swag.printbc]
#test
{
    catch th1.init(&entry1)
    catch thread.Thread.init(&th2, &entry2)
    catch th1.start()
    catch th2.start()
    th2.wait()
    //@print("end\n")
}
