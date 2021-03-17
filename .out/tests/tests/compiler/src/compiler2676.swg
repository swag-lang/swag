#test
{
    ptr := @{&toto}
    //@assert(ptr.item0() == 666)
}

func toto(x: s64) => 666