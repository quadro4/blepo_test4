/*
 Border pixels: handling border pixels is indispensible for chamfering distance and gradient magnitude,
avoiding extra segment in non marker watershed and the unwanted skewness. 
I simply add a outline of white boundary and ignoring area connected with whiteboundary.

 Convert: convert an image in Gradient Magnitude from float to Gray, convert function is not enough.
 Considering the difference of flaot value is needed.
 Gray(x,y) = (Float(x,y) - float_min)/(float_max - f_loat_min)*(gray_max - gray_min) + gray_min

*/

//Vs2012 update4

#include <afxwin.h>  // necessary for MFC to work properly
//#include <stdio.h>  

#include <math.h> 
#include "test.h"
#include "../../src/blepo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace blepo;
using namespace blepo_ex;


int main(int argc, const char* argv[], const char* envp[])
{
	
	HMODULE hModule = ::GetModuleHandle(NULL);
	if (hModule == NULL || !AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
	{
		printf("Fatal Error: MFC initialization failed (hModule = %x)\n", hModule);
		return 1;
	}
	


	//print info	
	printf("argc = %d\n", argc);
	for (int i=0; i<argc ; i++) 
	{
		printf("argv[%d]=%s\n\n", i, argv[i]);
	}


	int treshold_value_holes=72;
	int treshold_value_cells_small=34;
	
	printf("Note: suggested treshold_value=%d for holes.pgm\n", treshold_value_holes);
	printf("Note: suggested treshold_value=%d for cells_small.pgm\n", treshold_value_cells_small);
	printf("Note: threshold would output inverse color\n");


	//halt condition
	if( argc < 3)	
	{
		printf("\nError: lack enough command parameters \n");
		printf("Act: Program halt, Please close \n");
		EventLoop();
		return 0;
		//third_commandpara_specified=1;		
	}
	else if(argc > 3)
	{
		printf("\nWarning: more command parameters than use \n");
	}
	
	

	/*
	a.	Reads 1 command-line parameter, which we will call filename.  
	b.	Loads filename from the blepo/images directory into a Grayscale or BGR image and displays it in a figure window.
	*/
	CString path_blepo_images ="../../images/";
	CString filename_input1=argv[1];//loading file
	CString filename_input2=argv[2];//loading threshold value
	CString filename_forloading_origin = path_blepo_images + filename_input1;


	int status_filename=initial_filename_recognize(path_blepo_images, filename_input1);
	
	if( status_filename==1 )
	{
		printf("\nError: File cannot be found : %s \n",filename_forloading_origin );	
		printf("Act: Program halt, Please close \n");
		EventLoop();
		return 0;
	}
	
	
	//Val initial 
	int treshold_value=_ttoi(filename_input2);
	printf("Note: Input treshold_value is %d \n",treshold_value);
	
	if(treshold_value == 0)
	{
		
		if(filename_input1== "holes.pgm")	treshold_value=72;
		else if(filename_input1== "cells_small.pgm ")	treshold_value=35;
		else treshold_value=50;
		printf("Note: Lack of threshold value, would choose initial value %d\n",treshold_value);
	}


	printf("\n Start: \n");
	
	//a.
	// Loading filename1 2 and show it 

	ImgGray img_loaded_origin_g;
	


	//Figure and title
	Figure fig_loaded_image;
	fig_loaded_image.SetTitle("Loaded Image Origin");
	
	Load(filename_forloading_origin, &img_loaded_origin_g);
	fig_loaded_image.Draw(img_loaded_origin_g);


	/*b.	The very first thing the program should do is print a suggested threshold value 
		for both images that we will be using (holes.pgm and cells_small.pgm).  */

	ImgBinary img_threshold;//binary
	

	
	
	










	//initial
	img_threshold.Reset( img_loaded_origin_g.Width(), img_loaded_origin_g.Height() );
	Set(&img_threshold,1);


	//For 1
	for( int y=1; y< img_loaded_origin_g.Height()-1; y++ ) 
	{
		for( int x=1; x< img_loaded_origin_g.Width()-1; x++ ) 
		{		
			if( img_loaded_origin_g(x,y)>treshold_value)
			{	
				
				img_threshold(x,y)=0;
			}
		}
	}




	
	Figure fig_threshold;
	
	fig_threshold.SetTitle("threshold with reverse color");
	fig_threshold.Draw(img_threshold);
	

	/*d.	Compute the chamfer distance.
*/

	
	 ImgFloat img_threshold_f;
	 ImgInt img_threshold_int;
	 ImgGray img_threshold_g;

	 Convert(img_threshold,&img_threshold_int);
	 Convert(img_threshold,&img_threshold_f);
	 Convert(img_threshold,&img_threshold_g);

	 ImgFloat img_chamgering_distance;
	 ImgGray img_chamgering_distance_g;
	
	 const int ratio_chamfer=50;
	 if (  Chamfering_distance_compute(img_threshold_f, ratio_chamfer,&img_chamgering_distance) ==0 )  printf("d. 8 neighbours Chamfer with ratio %d\n",ratio_chamfer);
	 //if ( Chamfering_distance_compute(img_threshold_g, ratio_chamfer, &img_chamgering_distance_g) ==0 )  printf("d. \n");
	 
	

	
	Convert(img_chamgering_distance,&img_chamgering_distance_g);

	Figure fig_chamgering_distance;
	fig_chamgering_distance.SetTitle("img_chamgering_distance");
	fig_chamgering_distance.Draw(img_chamgering_distance_g);



	
	//non-markered-watershed
	ImgInt img_watershed;
	const int non_marker_based=1;
	if (  nonmarker_based_watershed_cal( img_chamgering_distance_g, &img_watershed, non_marker_based)  ==0 )  printf("d. non-markered-watershed \n");
	//WatershedSegmentation(img_chamgering_distance_g, &img_watershed, 0);

	Figure fig_nonwatershed;
	fig_nonwatershed.SetTitle("nonwatershed");
	fig_nonwatershed.Draw(img_watershed);

	



	//edge detection

	ImgInt img_edge_detection;
	if (  edge_detection( img_watershed, &img_edge_detection)   ==0 )  printf("d. edge detection \n");
	
	Figure fig_edge_detection;
	fig_edge_detection.SetTitle("edge_detection");
	fig_edge_detection.Draw(img_edge_detection);


	//get markers
	ImgInt img_markers;
	Or(img_threshold_int,img_edge_detection, &img_markers);


	for( int y=0; y< img_markers.Height(); y++ ) 
	{
		for( int x=0; x< img_markers.Width(); x++ ) 
		{	
			img_markers(x,0) =0;
			img_markers(0,y) =0;
			img_markers(img_markers.Width()-1,y) =0;
			img_markers(x,img_markers.Height()-1) =0;

		}//for x
	}//for y

	Figure fig_markers;
	fig_markers.SetTitle("markers");
	fig_markers.Draw(img_markers);






	//gradient
	float sigma_value=1.5f;
	int half_width=1;
	ImgFloat img_magnitude_f;
	ImgInt img_magnitude_int;
	ImgGray img_magnitude_g;

	
	//GradMagSobel(img_loaded_origin_g, &img_magnitude_g);

	if (  gradient_compute( img_loaded_origin_g, sigma_value, &img_magnitude_f, &half_width) ==0 )  printf("gradient \n");
	
	//make sure convert result has enough 0 pixel

    for (float*  pixel_pointer = img_magnitude_f.Begin() ; pixel_pointer != img_magnitude_f.End() ; pixel_pointer++)  
	{	
		if(*pixel_pointer<1.0f)  *pixel_pointer= 0.0f;
	}
	Convert(img_magnitude_f, &img_magnitude_g,1);

	Figure fig_magnitude;
	fig_magnitude.SetTitle("magnitude quantized");
	fig_magnitude.Draw(img_magnitude_f);


	




	/*e.	Perform the marker-based watershed algorithm.*/
	ImgInt img_markered_watershed;
	const int marker_based=1;
	//if (  watershed_compute( img_magnitude_g, &img_markered_watershed, marker_based)  ==0 )  printf("d. non-markered-watershed \n");
	
	if (  marker_based_watershed_cal( img_magnitude_g, img_markers,&img_markered_watershed, marker_based,half_width)  ==0 )  printf("e. markered-watershed \n");

	//WatershedSegmentation(img_magnitude_g, &img_markered_watershed, 0);

	Figure fig_marker_based_watershed;
	fig_marker_based_watershed.SetTitle("marker_based_watershed");
	fig_marker_based_watershed.Draw(img_markered_watershed);

	//find edge

	ImgBgr img_markered_watershed_edge;
	ImgBgr img_final_edge_on_origin;
	if (  edge_detection( img_markered_watershed, &img_markered_watershed_edge, Bgr(0,255,0))   ==0 )  printf("e. markered-watershed_edge \n");

	ImgBgr img_loaded_origin_bgr;
	Convert(img_loaded_origin_g,&img_loaded_origin_bgr);
	Or(img_loaded_origin_bgr,img_markered_watershed_edge, &img_final_edge_on_origin);

	Figure fig_final_edge_on_origin;
	fig_final_edge_on_origin.SetTitle("marker_based_watershed_edge");
	fig_final_edge_on_origin.Draw(img_final_edge_on_origin);



	EventLoop();
	return 0;
}










