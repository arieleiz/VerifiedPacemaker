//This file was generated from (Commercial) UPPAAL 4.0.14 (rev. 5615), May 2014

/*

*/
A[] not Observer_AV.BAD2

/*

*/
A[] not Observer_AV.BAD1

/*

*/
A[] Observer_AV.S5 imply Observer_AV.t >= cur_pvab

/*

*/
A[] Observer_VRP.S3 imply Observer_VRP.t >= cur_vrp

/*

*/
A[] Observer_URI.S3 imply Observer_URI.t >= (cur_avi + cur_avi_ext + cur_pvarp)

/*

*/
A[] Observer_URI.S3 imply Observer_URI.t >= cur_uri

/*

*/
A[] Observer_LRI.S3 imply Observer_LRI.t <= cur_lri

/*

*/
A[] PM_Pacer_AA.PaceV imply PM_Leds_Vpace.On

/*

*/
A[] PM_Pacer_AA.PaceA imply PM_Leds_Apace.On

/*

*/
A[] !deadlock

/*

*/
A<> Observer_PS.Asignalled

/*

*/
A<> Observer_PS.Vsignalled
