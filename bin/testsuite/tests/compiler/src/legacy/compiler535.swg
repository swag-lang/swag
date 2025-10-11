struct point
{
    x:     s32 = 10
    y:     s32 = 200
}

impl point
{
    /*func opInit(me)
	{
		me.x = 10
		me.y = 20
		@print("toto\n")
	}*/
}

//#[Swag.PrintBc]
#test
{
    var pt: point
    @assert(pt.x == 10)
    @assert(pt.y == 200)
}
