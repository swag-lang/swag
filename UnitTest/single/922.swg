#module module922

func average(x: any)
{
	t := @dataof(x)
}

#[swag.printbc]
#test
{
	average(1)
}
