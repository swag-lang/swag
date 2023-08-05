#global testerror
struct A {}

func set(using a: *A) {}
func get(using a: *A) => 0

var a: A

func driver()->*A { return &a; }

func set(x: s32) {}
func set1(x: s32) = driver().set(x)