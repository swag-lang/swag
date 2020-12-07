struct pt
{
    x, y: s32
}

//#[swag.printbc]
//#unittest error
#test
{
	var x: {s32, s32} = @{666}
    @assert(x.item0 == 666)
    @assert(x.item1 == 0)
}
