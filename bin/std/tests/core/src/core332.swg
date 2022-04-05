
using Core

func fibR(n: s32)->s32
{
    if n < 2 return n
    return fibR(n - 1) + fibR(n - 2)
}

//#[Swag.PrintBc]
func fibI(n: u32)->u32
{
    last := 0'u32
    cur := 1'u32
    loop i: n-1
    {
        tmp := cur
        cur +=,safe last
        last = tmp
    }

    return cur
}

#test
{
    var t: Time.Stopwatch

    {
        t.start()
        f1 := fibR(31)
        t.stop()
        @print("rec ", f1, " ", t.elapsedMilliseconds(), "\n")
	}

    var tot: u64
    loop 6
    {
        t.restart()
        f1 := fibI(6511134)
        t.stop()
        tot += t.elapsedMilliseconds()
    }

    @print("loop ", tot/6, "\n")
}