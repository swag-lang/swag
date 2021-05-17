using core.random

#test
{
    var rng: Rng
    //rng.seedNow()
    loop 10
        console.printf("% ", rng.nextU32())
}