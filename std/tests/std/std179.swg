#import "std"
using std

var th1, th2: thread.Thread

func entry1(th: thread.Thread)
{
}

func entry2(th: thread.Thread)
{
    th1.wait()
}

//#[swag.printbc]
#test
{
    /*th1 = thread.create(&entry1)
    th2 = thread.create(&entry2)
    th1.start()
    th2.start()
    th2.wait()*/
}
