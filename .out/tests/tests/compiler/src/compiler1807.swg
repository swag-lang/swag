const C = plus1()
#[Swag.constexpr]
{
    func plus1() => plus2()
    func plus2() => plus3()
    func plus3() => plus4()
    func plus4() => plus5()
    func plus5() => plus6()
    func plus6() => 1
}
