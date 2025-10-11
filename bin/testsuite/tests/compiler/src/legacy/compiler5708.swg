alias Bool = bool
alias Int  = s32

#test
{
    func chooseStepFunction(backward: Bool)->func(Int)->Int
    {
        func stepForward(input: Int)  => input + 1
        func stepBackward(input: Int) => input - 1
        return backward ? &stepBackward : &stepForward
    }

    var currentValue     = -4
    let moveNearerToZero = chooseStepFunction(backward: currentValue > 0)
    while currentValue != 0
    {
        //@print(currentValue, "... ")
        currentValue = moveNearerToZero(currentValue)
    }

    //@print("zero!")
}
