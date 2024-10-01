#global skip
var G = 0

#[Swag.PrintGenBc]
#test
{
    let nc = G
    if nc
    {
        var p: *s32
        if !nc:
            dref p = 1
    }
}