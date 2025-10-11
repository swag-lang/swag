#test
{
    let str = "10"
    let a   = 50
    let b   = 150
    let v:  func||() = func|str, a, b|()
    {
        @assert(str == "10")
        @assert(a == 50)
        @assert(b == 150)
    }

    v()
}
