#global testerror "Err0194"
struct Duration
{
    val: f32
}

impl Duration
{
    /*mtd(op: string) opAffectLiteral(v: f32)
    {
        #if op == "s"
            val = v * 1000
        #elif op == "ms"
            val = v
        #else
            #assert false
    }*/
}

#test
{
    var x:  Duration = 1.0's
}