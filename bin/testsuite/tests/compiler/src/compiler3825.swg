	/* salut la foule
		/* */
	*/

//#[Swag.PrintBc]
#test
{
    a := 1'u8
    b := 2'u8
    (a, b) = @{b, a}
    @assert(a == 2)
    @assert(b == 1)
}