#module module178;
#unittest pass semantic;

namespace A{}
#unittest error;

#assert A && A;
