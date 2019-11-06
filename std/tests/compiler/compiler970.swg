struct pt
{
    x, y: s32
}

//#[swag.printbc]
//#unittest error
#test
{
	var x: {s32, s32} = {666}
    @assert(x.val0 == 666)
    @assert(x.val1 == 0)
}
