struct A
{
	tbl: *AI
}

struct AI
{
	call: func(me: *A, x: s32 = 0)
}

func myCall(me: *A, x: s32)
{
}

#test
{
	var a: A
	var ai: AI
	
	a.tbl = &ai
	a.tbl.call = &myCall
	a.tbl.call(&a)
}