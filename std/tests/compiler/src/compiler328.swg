#module "compiler328"
#unittest pass syntax;

#test
{
	var a: [4] s32 = @{0};
	var a: [4] s32 = @{0, 1, 2, 4};
	var a: [4] s32 = @{{0, 1}, 2, 4};
	var a: [4] s32 = @{{0, {1}}, 2, A + 5};
}
