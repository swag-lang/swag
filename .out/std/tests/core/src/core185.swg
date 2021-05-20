
using Core.Time

//#[Swag.printbc]
#test
{
    ts := TimeSpan.now()
    tt := ts.totalSeconds()
    //@print(tt)

    //Threading.sleep(1000)

    ts1 := TimeSpan.now()
    tt1 := ts1.totalSeconds() - tt
    //@print(tt1)
}
