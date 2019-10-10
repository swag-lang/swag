#module module1064

func fibonacci(num: s32) -> s32 
{
    if num < 2 
	{
        return num
    } 
	else 
	{
        return fibonacci(num - 1) + fibonacci(num - 2)
    }
}