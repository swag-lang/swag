/* salut la foule
		/* */
	*/

//#[Swag.PrintBc]
#test
{
    var a = 1'u16
    var b = 2'u16
    (a, b) = {b, a}
    @assert(a == 2)
    @assert(b == 1)
}
