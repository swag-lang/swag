using Swag

func main1()->s32 { return 77; }

#[ConstExpr]
func main2()->s32 { return 666; }

//#[PrintBc]
func main()
{
    let a = #run main1()
    let b = main1()
    let c = main2()
    @assert(a == 77)
    @assert(b == 77)
    @assert(c == 666)
}

#run
{
    main()
}
