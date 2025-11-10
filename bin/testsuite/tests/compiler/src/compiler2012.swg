#[Swag.Overload]
public func(T) toto2012()
{
    var cb  = func(x, y: T = 0) { return x; }
    var cb1 = func(x, y: T = 0) => x
}
