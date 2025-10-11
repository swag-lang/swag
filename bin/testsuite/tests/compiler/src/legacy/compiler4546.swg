enum A { R, G, B }
enum AA { R, G, B }

#[Swag.Overload]
func toto(x: AA) {}

#[Swag.Overload]
func toto(x: A) {}
/*
func titi()
{
    toto(.R)
}
*/
