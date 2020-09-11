#import "tests.compiler.foreign"

struct AA
{

}

#test
{
    x := 10'u32
    y := 5'u32
    r := 0
    loop (x - y) - 1
        r += 1
    @assert(r == 4)

    //var xx: compiler1965.AA
}