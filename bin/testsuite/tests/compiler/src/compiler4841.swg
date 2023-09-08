#global skip
#[Swag.Macro]
func Visit(str: const [..] u8)
{
    var ptr = @dataof(str)
    var count = @countof(str)
}

//#[Swag.PrintBc]
#test
{
	let str = "あああ"
    Visit(str)
    {
    }
}