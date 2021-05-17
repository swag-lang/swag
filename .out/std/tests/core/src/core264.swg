using core.random

//#[swag.printbc]
#test
{
	var rng: Rng
    loop 500
    {
        v := rng.nextU64(0, 256)
        @print(v," ")
        if v == 255 break
    }
}