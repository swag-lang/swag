using Swag

//#[PrintBc]
func main()
{
    let a: s32 = 666
    @assert(a == 666)
    let b: s64 = 555
    @assert(b == 555)
}

#run
{
    main()
}
