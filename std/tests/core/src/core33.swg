#import "core"

#test
{
	using core.mouse, core.keyboard
	while !isKeyPressed(Key.Space)
    {
        pt := getPos()
        //@print(cast(s64) pt.item0)
        //@print("\n")
        break
    }

	//@print("pressed")
	while isKeyPressed(Key.Space) break

	//@print("unpressed")
}