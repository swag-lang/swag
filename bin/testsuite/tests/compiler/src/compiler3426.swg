//#[Swag.PrintGenBc]
#test
{
    let str = "10"
    let v:  func||() = func|str|()
    {
        @assert(str == "10")
    }

    v()
}
