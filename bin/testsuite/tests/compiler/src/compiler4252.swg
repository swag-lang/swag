#test
{
    let x = "abcd"
    let v = 2
    let y = x[..<v]
    @assert(y == "ab")
}
