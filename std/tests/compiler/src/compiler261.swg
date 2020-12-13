
;
using swag;

#unittest error;
func main ()->s32
{
	if(true)
		return 0;
}

#unittest error;
func main1 ()->s32
{
	while(true)
		return 0;
}

#unittest error;
func main2 ()->s32
{
	if(true)
	{
	}
	else
	{
		return 0;
	}
}
