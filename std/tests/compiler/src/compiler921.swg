

#unittest error
func average(x: any)
{
	t := @dataof(any)
}

//#[swag.printbc]
#test
{
	average(1)
}
