
using Core.Time

//#[Swag.PrintBc]
#test
{
    var ts = TimeSpan.now()
    let tt = ts.totalSeconds()
    //@print(tt)

    //Threading.sleep(1000)

    var ts1 = TimeSpan.now()
    let tt1 = ts1.totalSeconds() - tt
    //@print(tt1)
}
