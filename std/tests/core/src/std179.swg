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
    thread.create(&th1, &entry1)
    thread.create(&th2, &entry2)
    th1.start()
    th2.start()
    th2.wait()
    //@print("end\n")
}
