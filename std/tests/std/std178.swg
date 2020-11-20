#import "std"
using std

func entry(th: thread.Thread)
{
}

//#[swag.printbc]
#test
{
    var th: thread.Thread
    thread.create(&th, &entry)
    th.start()
    th.wait()
}
