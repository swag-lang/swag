struct pt
{
    x, y: s32
}

//#[Swag.PrintBc]
//#global testerror
#test
{
	var x: {s32, s32} = @{666}
    @assert(x.item0 == 666)
    @assert(x.item1 == 0)
}
