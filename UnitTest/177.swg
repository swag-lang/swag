#unittest module module177;
#unittest pass semantic;

namespace A{}

#unittest error;
#run A + A;
#unittest error;
#run !A;
#unittest error;
#run -A;