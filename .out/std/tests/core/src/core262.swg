using core.random

#[swag.printbc]
#test
{
    var rng: Rng
    //rng.seedNow()
    //loop 1
    {
        @bcdbg()
        @print(rng.nextU32())
        //console.printf("% ", rng.nextU32())
    }
}