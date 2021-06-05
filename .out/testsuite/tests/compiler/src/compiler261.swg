#global testerror;
;
using Swag;

func main ()->s32
{
	if(true)
		return 0;
}

#global testerror;
func main1 ()->s32
{
	while(true)
		return 0;
}

#global testerror;
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
