#import "std"
using std.time

//#[swag.printbc]
#test
{
    ts := TimeSpan.now()
    tt := ts.totalSeconds()
    //@print(tt)

    //thread.sleep(1000)

    ts1 := TimeSpan.now()
    tt1 := ts1.totalSeconds() - tt
    //@print(tt1)
}
