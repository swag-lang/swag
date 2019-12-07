#import "std"

#test
{
	using std.keyboard
	while !isKeyPressed(Key.Space) break
	
	//@print("pressed")
	while isKeyPressed(Key.Space) break

	//@print("unpressed")
}