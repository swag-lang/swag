func stepForward(input: s32)            => input + 1
func stepBackward(input: s32)           => input - 1
func chooseStepFunction(backward: bool) => backward ? &stepBackward : &stepForward

//#[Swag.PrintBc]
#test
{
    var currentValue     = 3
    let moveNearerToZero = chooseStepFunction(backward: currentValue > 0)

    var cpt = 0
    while currentValue != 0
    {
        cpt += currentValue
        currentValue = moveNearerToZero(currentValue)
    }

    @assert(cpt == 6)
}
