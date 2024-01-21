#global testerror "Err0343"
struct Duration
{
    val: f32
}

impl Duration
{
    /*mtd(op: string) opAffectSuffix(v: f32)
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