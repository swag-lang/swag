#[Swag.Macro]
func calculateSquare(num: #code s32)
{
    let result = #inject(num) * #inject(num)
    let str    = "Square of " ++ #stringof(num) ++ " is " ++ result
}

#test
{
    calculateSquare(5) // Output: Square of 5 is 25
    let x = 10
    calculateSquare(x) // Output: Square of x is 100
    calculateSquare(x + 2) // Output: Square of x + 2 is 144
}
