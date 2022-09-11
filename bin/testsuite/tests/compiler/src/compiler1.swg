	/* salut la foule
		/* */
	*/

struct Point
{
    x, y: f32
}

func titi() => 666

#[Swag.Optim("", false)]
func toto(x, y: f32, addr: *f32, slice: const [..] string)
{
    arr := @[10, 20, 30]
    @bcdbg()

    var pt: Point
    pt.x = x
    pt.y = y

    var pt1: Point
    pt1.x = x+1
    pt1.y = y+1

    arr1 := @[pt, pt1]
    arr2 := @[1, 2, 4]

    tp := @typeof(pt)

    @print(pt.x, pt.y)
}

#test
{
    x := 5.0
    toto(1.0, 2.0, &x, @["toto", "titi"])
}