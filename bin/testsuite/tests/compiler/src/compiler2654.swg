//func(T) min(x, y: T) => x < y ? x : y
//func(T) min(x, y, z: T) => min(min(x, y), z)

func(T) titi(x, y: T) => x < y ? x : y
func(T) min(x, y, z: T) => titi(titi(x, y), z)
