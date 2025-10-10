#global testerror "Saf0033"

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#[Swag.Sanity(true)]
func toto()
{
    var x: [10] s32
    @memset(&x, 0, 1'u64+#sizeof(x))
}
