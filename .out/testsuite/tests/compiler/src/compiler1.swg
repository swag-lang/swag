	/* salut la foule
		/* */
	*/

struct AA
{

}

impl AA
{
    func toto(using self)
    {
        @print("toto")
    }
}

#test
{
    var x: AA
    x.toto()
}