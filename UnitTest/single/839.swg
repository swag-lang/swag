#module module839

#[swag.printbc]
#test
{
	i := 0
	while(i < 10)
	{
		defer @print(i)
		i += 1
	}
}