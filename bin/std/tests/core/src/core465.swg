using Core
var g_Rng: Random.Rng

//#[Swag.PrintBc]
#test
{
    var a: [2] s32
    g_Rng.shuffle(cast() a)
}
