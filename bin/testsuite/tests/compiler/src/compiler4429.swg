//#global skip
using Swag

var G = 0

#test
{
	x := func() {
		G = 1
	}
	
	r := ref &x
	r()
	@assert(G == 1)
}
