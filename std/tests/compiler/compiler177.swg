#module "compiler177"
;

namespace A{}

#unittest error;
#assert A + A;
#unittest error;
#assert !A;
#unittest error;
#assert -A;