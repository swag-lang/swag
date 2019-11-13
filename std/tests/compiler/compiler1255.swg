#module compiler1255
#unittest error

#test
{
	x, x := 666
    @assert(x == 666)
    @assert(y == 666)    
}