#global testerror "Err0619"
struct A {}

#[Swag.Overload]
func set(using a: *A) {}
func get(using a: *A) => 0

var a: A

func driver()->*A { return &a; }

#[Swag.Overload]
func set(x: s32) {}
func set1(x: s32) = driver().set(x)