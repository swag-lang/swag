#test
{
    let a  = 10
    let b  = 20
    let v: func||(s32) = func|a, b|(x: s32)
    {
        @assert(a == 10)
        @assert(b == 20)
        @assert(x == 5)
    }

    v(5)
}
