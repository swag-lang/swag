#module compiler177
#unittest pass semantic;

namespace A{}

#unittest error;
#assert A + A;
#unittest error;
#assert !A;
#unittest error;
#assert -A;