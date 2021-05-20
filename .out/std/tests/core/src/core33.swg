

#test
{
	using core.Input
	while !isKeyPressed(Key.Space)
    {
        pt := getMousePos()
        //@print(cast(s64) pt.item0)
        //@print("\n")
        break
    }

	//@print("pressed")
	while isKeyPressed(Key.Space) break

	//@print("unpressed")
}