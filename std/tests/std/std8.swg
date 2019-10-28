#import "std"
using std

//#[swag.printbc]
#test
{
	using console
	setTextColor(Color.Red)
	setBackColor(Color.Black)
	@print("toto\n")
	resetColor()
}