struct A
{
    x:   [3] s32
}

impl A
{
    #[Swag.Mixin]
    mtd toto(index0: u64, value: s32)
    {
        #ast "me.x[index0] " ++ "+=" ++ " value"
    }
}

#test
{
    var a: A
    a.x[1] = 5
    a.toto(1, 661)
    a.toto(1, 1)
    @assert(a.x[1] == 667)
}
