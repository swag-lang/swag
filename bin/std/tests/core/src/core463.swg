#global skip
using Core

//const MAX = 440_000_000
const MAX = 10_000_000

var cache: [10] s32 = 1

#[Swag.PrintBc]
func is_munchausen(number: s32)->bool
{
    var n = number
    var total = 0

    while n > 0
    {
        let digit = n % 10
        total += cache[digit]
        if total > number:
            return false
        n = n / 10
    }

    return total == number
}

//#[Swag.Inline]
func set_cache()
{
    cache[0] = 0
    for i in 1 to 9:
        cache[i] = cast(s32) Math.pow(cast(f32) i, cast(f32) i)
}

//#[Swag.PrintBc]
#test
{
    Time.Stopwatch.scopeMeasure()
    set_cache()
    for i in MAX
    {
        if is_munchausen(i):
            Console.printf("%\n", i)
    }
}