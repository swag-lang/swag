
using swag

func toto()
{
	@assert(#function == "toto")

}
//#[swag.printbc]
#test
{
	toto()
}
