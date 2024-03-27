/* salut la foule
		/* */
	*/

var G = 0
#premain
{
    G += 1
}

#init
{
}

#test
{
    @assert(G == 1)
}
