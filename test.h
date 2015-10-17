
//Vs2012 Update4

#pragma once
#include <afxwin.h>  // necessary for MFC to work properly
#include <math.h> 
#include <stdio.h> 
#include <queue>


#include "../../src/blepo.h"


using namespace std;
using namespace blepo;
using namespace blepo_ex;
//using namespace blepo_ex;


struct node
	{
		int x;
		int y;
	};


int initial_filename_recognize( const CString path_input, const CString filename_input)
{
	

	if(   fopen(path_input+filename_input,"r")==0   )
	  { return 1; }
	
	else return 0;
				
}


int gaussian_kernel_compute(const float sigma_value, ImgFloat* gaussian_kernel)
{
	float f=2.5;
	int kernel_halfwidth= Round( f * sigma_value -0.5); //
	int kernel_width=2*kernel_halfwidth +1;

	//input protection
	if(kernel_width>=100 || sigma_value==0)
	{
		return 1;
		
	}


	//ImgFloat gaussian_kernel;

	(*gaussian_kernel).Reset(kernel_width,1);
	Set(gaussian_kernel,0);

	float gaussian_sum=0;
	float sum1=0;
	
	//Build Gaussian  kernel
	for(int ctn=0; ctn<kernel_width ; ctn++)
	{
		(*gaussian_kernel)(ctn,0) = exp( 
						-(ctn-kernel_halfwidth) * (ctn-kernel_halfwidth)  
					   /(2 * sigma_value * sigma_value) );
		gaussian_sum+=(*gaussian_kernel)(ctn,0) ;
	
	}

	for(int ctn=0; ctn<kernel_width ; ctn++)
	{
		(*gaussian_kernel)(ctn,0)/= gaussian_sum;
		sum1+=(*gaussian_kernel)(ctn,0);
	
	}


	/*printf("b.\n");
	printf("kernel_halfwidth = %d \n", kernel_halfwidth);
	printf("gaussian_sum = %f \n", gaussian_sum);
	printf("Sum1 = %f \n", sum1);
	for(int ctn=0; ctn<kernel_width ; ctn++)
	{
		printf("gaussian_kernel[%d] = %f \n",ctn , (*gaussian_kernel)(ctn,0) );
	
	
	}*/



	return 0;

}



int gaussian_kernel_deriv_compute(const float sigma_value, ImgFloat* gaussian_kernel_deriv)
{
	float f=2.5;
	int kernel_halfwidth= Round( f * sigma_value -0.5); //
	int kernel_width=2*kernel_halfwidth +1;

	//input protection
	if(kernel_width>=100 || sigma_value==0)
	{
		return 1;
		
	}



	(*gaussian_kernel_deriv).Reset(kernel_width,1);
	Set(gaussian_kernel_deriv,0);

	float gaussian_deri_sum=0;
	float sum2=0;

	for(int ctn=0; ctn<kernel_width ; ctn++)
	{
		(*gaussian_kernel_deriv)(ctn,0) = (ctn-kernel_halfwidth)
				*exp(  -(ctn-kernel_halfwidth) * (ctn-kernel_halfwidth)  
					   /(2 * sigma_value * sigma_value) );

		gaussian_deri_sum+=(*gaussian_kernel_deriv)(ctn,0) * ctn;
	
	}

	for(int ctn=0; ctn<kernel_width ; ctn++)
	{
		(*gaussian_kernel_deriv)(ctn,0)= (-1)*(*gaussian_kernel_deriv)(ctn,0)/gaussian_deri_sum;
		sum2+=(*gaussian_kernel_deriv)(ctn,0);
	
	}

	
	/*printf("\n\n");
	printf("gaussian_deri_sum = %f \n", gaussian_deri_sum);
	printf("Sum2 = %f \n", sum2);
	printf("With flip \n");
	for(int ctn=0; ctn<kernel_width ; ctn++)
	{
		
		printf("gaussian_deri_kernel[%d] = %f \n",ctn , (*gaussian_kernel_deriv)(ctn,0));
	}*/

	return 0;
}



//convolution_x   with kernel
void separable_convolution_x( const ImgFloat &img_in,  const int kernel_width, const ImgFloat gaussian_kernel, ImgFloat* img_out ) 
{


	(*img_out).Reset( img_in.Width(),img_in.Height() );
	Set(img_out, 0.0);

	
	for( int y=0; y< img_in.Height(); y++ ) 
	{
		for( int x=(kernel_width-1)/2; x< img_in.Width()-(kernel_width-1)/2; x++ ) 
		{
			float tmp_value=0;
			for(int ctn=0; ctn<kernel_width ; ctn++)
			{

				if(	  ( (x+ (kernel_width-1)/2 - ctn) < 0 ) 
					||( (x+ (kernel_width-1)/2 - ctn) >= img_in.Width() )   
				  )

				{
					tmp_value+=gaussian_kernel(ctn,0) * 0.0f;				
				}

				else 
				{
					tmp_value+=gaussian_kernel(ctn,0) * img_in(x+ (kernel_width-1)/2 - ctn,y);
				}

			}//for ctn

			(*img_out)(x,y)=tmp_value;
			

		}//for x
	}//for y

	//return 0;

}



//convolution_y   with kernel
void separable_convolution_y( const ImgFloat &img_in,  const int kernel_width, const ImgFloat gaussian_kernel, ImgFloat* img_out ) 
{


	(*img_out).Reset( img_in.Width() , img_in.Height() );
	Set(img_out, 0.0);

	for( int x=0; x< img_in.Width(); x++) 
	{
		for(  int y=(kernel_width-1)/2; y< img_in.Height()-(kernel_width-1)/2; y++ ) 
		{
			float tmp_value=0;

			for(int ctn=0; ctn<kernel_width ; ctn++)
			{

				if(	  ( (y+ (kernel_width-1)/2 - ctn) < 0 ) 
					||( (y+ (kernel_width-1)/2 - ctn) >= img_in.Height() )   
				  )

				{
					tmp_value+=gaussian_kernel(ctn,0) * 0.0f;				
				}

				else 
				{
					tmp_value+=gaussian_kernel(ctn,0) * img_in(x,y + (kernel_width-1)/2 - ctn );
				}

			}//for ctn
			
			(*img_out)(x,y)=tmp_value;
			

		}//for x
	}//for y

	//return 0;

}









//convolution x and y  with kernel
int separable_convolution_2d( const ImgGray &img_in_g,  
							  const int kernel_width,
							  const ImgFloat gaussian_kernel ,
							  const ImgFloat gaussian_deri_kernel, 
							  ImgFloat* img_out, const int mode=0 ) 
{
	ImgFloat img_in_f;
	ImgFloat img_mid;
	

	Convert(img_in_g, &img_in_f);


	if(mode == 0) // x  y'
	{
		separable_convolution_y( img_in_f,  gaussian_deri_kernel.Width(), gaussian_deri_kernel, &img_mid );
		separable_convolution_x( img_mid,  gaussian_kernel.Width(), gaussian_kernel,  img_out);
		
		return 0;
		
	}

	else if(mode == 1)// y x'
	{
		separable_convolution_x( img_in_f,  gaussian_deri_kernel.Width(), gaussian_deri_kernel, &img_mid );
		separable_convolution_y( img_mid,  gaussian_kernel.Width(), gaussian_kernel,  img_out);
		
		return 0;
	}

	else
	{
		return 1;
	}
		
}


//make magnitude and anlge figure
int grad_magnitude_angle( const ImgFloat &img_in_x,  const ImgFloat &img_in_y,  ImgFloat* img_out_magnitude,  ImgFloat* img_out_angle ) 
{

	(*img_out_magnitude).Reset( img_in_x.Width(),img_in_x.Height() );
	Set(img_out_magnitude, 0.0);

	(*img_out_angle).Reset( img_in_x.Width(),img_in_x.Height() );
	Set(img_out_angle, 0.0);


	for( int y=0; y< img_in_x.Height(); y++ ) 
	{
		for( int x=0; x< img_in_x.Width(); x++ ) 
		{
			//(*img_out_magnitude)(x,y)=sqrt( abs( img_in_x(x,y) )*abs( img_in_x(x,y) ) +abs( img_in_y(x,y) )*abs( img_in_y(x,y) ) );
			(*img_out_magnitude)(x,y)=max(abs( img_in_x(x,y) ), abs( img_in_y(x,y)) );
			(*img_out_angle)(x,y)=atan2(img_in_y(x,y),img_in_x(x,y) );
			

		}//for x
	}//for y

	return 0;
	
}


//make magnitude and anlge figure
int grad_magnitude( const ImgFloat &img_in_x,  const ImgFloat &img_in_y, const int* half_width, ImgFloat* img_out_magnitude) 
{

	(*img_out_magnitude).Reset( img_in_x.Width(),img_in_x.Height() );
	Set(img_out_magnitude, 1.0f);

	for( int y=*half_width; y< img_in_x.Height()-*half_width; y++ ) 
	{
		for( int x=*half_width; x< img_in_x.Width()-*half_width; x++ ) 
		{
			//(*img_out_magnitude)(x,y)=sqrt( abs( img_in_x(x,y) )*abs( img_in_x(x,y) ) +abs( img_in_y(x,y) )*abs( img_in_y(x,y) ) );
			(*img_out_magnitude)(x,y)=max(abs( img_in_x(x,y) ), abs( img_in_y(x,y)) );
		}//for x
	}//for y

	return 0;
	
}



int gradient_compute( const ImgGray &img_in, const float sigma_value, ImgFloat* img_out, int* half_width)
{
	ImgFloat gaussian_kernel;
	ImgFloat gaussian_kernel_deriv;
	//float kernel_width=0;
	

	ImgFloat img_convolution_xd;
	ImgFloat img_convolution_yd;

	
	



	//Guassian kernel
	if ( gaussian_kernel_compute(sigma_value, &gaussian_kernel) ==0)   printf(" G_k ");
	
	//Guassian deri kernel
	if ( gaussian_kernel_deriv_compute(sigma_value, &gaussian_kernel_deriv) ==0)   printf(" G_d_k ");
	

	//Gd x
	if ( separable_convolution_2d( img_in,  gaussian_kernel.Width() , gaussian_kernel ,gaussian_kernel_deriv , &img_convolution_xd , 1) ==0)   printf(" Gx ");
	
	//Gd y
	if ( separable_convolution_2d( img_in,  gaussian_kernel.Width() , gaussian_kernel , gaussian_kernel_deriv, &img_convolution_yd , 0) ==0) printf(" Gy ");
	
	/*Figure gx,gy;
	gx.SetTitle("gx");
	gy.SetTitle("gy");
	gx.Draw(img_convolution_xd);
	gy.Draw(img_convolution_yd);
*/
	//magnitude
	*half_width=(gaussian_kernel.Width()-1)/2;

	if ( grad_magnitude( img_convolution_xd,  img_convolution_yd,half_width,  img_out) ==0) printf(" Mag ");;

	
	


	return 0;
}
void inner_marker_watershed_floodfill_4neighbour_wt_queue_boundaryoffset( const ImgInt &img_in, 
																		  const int gray_level_for_now, 
																		  queue<node> *point_xy ,
																		  ImgInt* img_out, 
																		  const int boundaryoffset=0)
{
	node pixel_for_now,pixel_new_frontier;	
	int initial_val=-1;

	while ( !(*point_xy).empty() )
	{

		int status=0;
			

		pixel_for_now=(*point_xy).front();
		//pixel_for_now.x
		//pixel_for_now.y

		(*point_xy).pop();

		int expend_val=(*img_out)(pixel_for_now.x,pixel_for_now.y);

		
		if (pixel_for_now.x > 1 
			&& img_in(pixel_for_now.x-1,pixel_for_now.y) <= gray_level_for_now 
			&& (*img_out)(pixel_for_now.x-1,pixel_for_now.y) == initial_val)  
		{	

			pixel_new_frontier.x=pixel_for_now.x-1;
			pixel_new_frontier.y=pixel_for_now.y;
			(*point_xy).push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x-1,pixel_for_now.y) = expend_val;
		}
		

		if (pixel_for_now.x < img_in.Width()-2 
			&& img_in(pixel_for_now.x+1,pixel_for_now.y) <= gray_level_for_now 
			&& (*img_out)(pixel_for_now.x+1,pixel_for_now.y) == initial_val)  
		{	
			pixel_new_frontier.x=pixel_for_now.x+1;
			pixel_new_frontier.y=pixel_for_now.y;
			(*point_xy).push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x+1,pixel_for_now.y) = expend_val;
		}


		if (pixel_for_now.y > 1 
			&& img_in(pixel_for_now.x,pixel_for_now.y-1) <= gray_level_for_now 
			&& (*img_out)(pixel_for_now.x,pixel_for_now.y-1) == initial_val)  
		{	

			pixel_new_frontier.x=pixel_for_now.x;
			pixel_new_frontier.y=pixel_for_now.y-1;
			(*point_xy).push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x,pixel_for_now.y-1) = expend_val;
			
		}
		

		if (pixel_for_now.y  < img_in.Height()-2 
			&&img_in(pixel_for_now.x,pixel_for_now.y+1) <= gray_level_for_now 
			&& (*img_out)(pixel_for_now.x,pixel_for_now.y+1) == initial_val)  
		{	

			pixel_new_frontier.x=pixel_for_now.x;
			pixel_new_frontier.y=pixel_for_now.y+1;
			(*point_xy).push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x,pixel_for_now.y+1) = expend_val;
		}
		
	}//while





}




void inner_nonmarker_watershed_floodfill_4neighbour_wt_queue_offset( const ImgInt &img_in, 
																	 const int gray_level_for_now, 
																	 queue<node> *point_xy ,
																	 ImgInt* img_out, 
																	 const int boundaryoffset=0)
{
	//scan range is 1->height-2 /  1->width-2
	
	//(*img_out).Reset(img_in.Width(), img_in.Height());
	
	
	//initial
	node pixel_for_now,pixel_new_frontier;	
	int initial_val=-1;

	while ( !(*point_xy).empty() )
	{

		int status=0;
			

		pixel_for_now=(*point_xy).front();
		//pixel_for_now.x
		//pixel_for_now.y

		(*point_xy).pop();

		int expend_val=(*img_out)(pixel_for_now.x,pixel_for_now.y);
		


		
		if (pixel_for_now.x >= boundaryoffset 
			&& img_in(pixel_for_now.x-1,pixel_for_now.y) == gray_level_for_now 
			&& (*img_out)(pixel_for_now.x-1,pixel_for_now.y) == initial_val)  
		{	

			pixel_new_frontier.x=pixel_for_now.x-1;
			pixel_new_frontier.y=pixel_for_now.y;
			(*point_xy).push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x-1,pixel_for_now.y) = expend_val;
		}
		


		if (pixel_for_now.x < img_in.Width()-boundaryoffset 
			&& img_in(pixel_for_now.x+1,pixel_for_now.y) == gray_level_for_now 
			&& (*img_out)(pixel_for_now.x+1,pixel_for_now.y) == initial_val)  
		{	
			pixel_new_frontier.x=pixel_for_now.x+1;
			pixel_new_frontier.y=pixel_for_now.y;
			(*point_xy).push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x+1,pixel_for_now.y) = expend_val;
		}


		if (pixel_for_now.y >= boundaryoffset 
			&& img_in(pixel_for_now.x,pixel_for_now.y-1) == gray_level_for_now 
			&& (*img_out)(pixel_for_now.x,pixel_for_now.y-1) == initial_val)  
		{	

			pixel_new_frontier.x=pixel_for_now.x;
			pixel_new_frontier.y=pixel_for_now.y-1;
			(*point_xy).push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x,pixel_for_now.y-1) = expend_val;
			
		}
	
		if (pixel_for_now.y  < img_in.Height()-boundaryoffset 
			&&img_in(pixel_for_now.x,pixel_for_now.y+1) == gray_level_for_now 
			&& (*img_out)(pixel_for_now.x,pixel_for_now.y+1) == initial_val)  
		{	

			pixel_new_frontier.x=pixel_for_now.x;
			pixel_new_frontier.y=pixel_for_now.y+1;
			(*point_xy).push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x,pixel_for_now.y+1) = expend_val;
			
		}
		

	
	}//while

}


void floodfill_4neighbour_wt_queue( const ImgInt &img_in, 
								    const int x, const int y,
									const int new_flood_val, 
								    ImgInt* img_out)
{
	//scan range is 0->height-1 /  0->width-1
	(*img_out).Reset(img_in.Width(), img_in.Height());
	
	
	


	//initial

	node pixel,pixel_for_now,pixel_new_frontier;

	queue<node> point_xy;

		


	if( new_flood_val == (*img_out)(x,y) )  return;

	(*img_out)(x,y) = new_flood_val;
	
	int proto_val=img_in(x,y);
	
	pixel.x=x;
	pixel.y=y;

	point_xy.push(pixel);
	
	while ( !point_xy.empty() )
	{

		int status=0;
			

		pixel_for_now=point_xy.front();
		//pixel_for_now.x
		//pixel_for_now.y

		point_xy.pop();

		//+1 +1 -1 -1
		if (pixel_for_now.x > 0 
			&& img_in(pixel_for_now.x-1,pixel_for_now.y) == proto_val 
			&& (*img_out)(pixel_for_now.x-1,pixel_for_now.y) != new_flood_val)  
		{	

			pixel_new_frontier.x=pixel_for_now.x-1;
			pixel_new_frontier.y=pixel_for_now.y;
			point_xy.push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x-1,pixel_for_now.y) = new_flood_val;
		}
		


		if (pixel_for_now.x < img_in.Width()-1 
			&& img_in(pixel_for_now.x+1,pixel_for_now.y) == proto_val 
			&& (*img_out)(pixel_for_now.x+1,pixel_for_now.y) != new_flood_val)  
		{	
			pixel_new_frontier.x=pixel_for_now.x+1;
			pixel_new_frontier.y=pixel_for_now.y;
			point_xy.push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x+1,pixel_for_now.y) = new_flood_val;
		}

		if (pixel_for_now.y > 0 
			&& img_in(pixel_for_now.x,pixel_for_now.y-1) == proto_val 
			&& (*img_out)(pixel_for_now.x,pixel_for_now.y-1) != new_flood_val)  
		{	

			pixel_new_frontier.x=pixel_for_now.x;
			pixel_new_frontier.y=pixel_for_now.y-1;
			point_xy.push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x,pixel_for_now.y-1) = new_flood_val;
			
		}
		

		if (pixel_for_now.y  < img_in.Height()-1 
			&&img_in(pixel_for_now.x,pixel_for_now.y+1) == proto_val 
			&& (*img_out)(pixel_for_now.x,pixel_for_now.y+1) != new_flood_val)  
		{	

			pixel_new_frontier.x=pixel_for_now.x;
			pixel_new_frontier.y=pixel_for_now.y+1;
			point_xy.push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x,pixel_for_now.y+1) = new_flood_val;
			
		}
		

		
	
	}//while
}



void floodfill_8neighbour_wt_queue( const ImgInt &img_in, 
								    const int x, const int y,
									const int new_flood_val, 
									ImgInt* img_out)
{
	//scan range is 0->height-1 /  0->width-1
	(*img_out).Reset(img_in.Width(), img_in.Height());
	
	//initial

	node pixel,pixel_for_now,pixel_new_frontier;

	queue<node> point_xy;

		


	if( new_flood_val == (*img_out)(x,y) )  return;

	(*img_out)(x,y) = new_flood_val;
	
	int proto_val=img_in(x,y);
	
	pixel.x=x;
	pixel.y=y;

	point_xy.push(pixel);
	
	while ( !point_xy.empty() )
	{

		int status=0;
			

		pixel_for_now=point_xy.front();
		//pixel_for_now.x
		//pixel_for_now.y

		point_xy.pop();

		//8 neighbour
		// x y +1 +1
		if (pixel_for_now.x < img_in.Width()-1 &&pixel_for_now.y  < img_in.Height()-1
			&& img_in(pixel_for_now.x+1,pixel_for_now.y+1) == proto_val 
			&& (*img_out)(pixel_for_now.x+1,pixel_for_now.y+1) != new_flood_val)  
		{	
			pixel_new_frontier.x=pixel_for_now.x+1;
			pixel_new_frontier.y=pixel_for_now.y+1;
			point_xy.push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x+1,pixel_for_now.y+1) = new_flood_val;
		}

		// x y -1 -1
		if (pixel_for_now.x > 0 && pixel_for_now.y > 0
			&& img_in(pixel_for_now.x-1,pixel_for_now.y-1) == proto_val 
			&& (*img_out)(pixel_for_now.x-1,pixel_for_now.y-1) != new_flood_val)  
		{	
			pixel_new_frontier.x=pixel_for_now.x-1;
			pixel_new_frontier.y=pixel_for_now.y-1;
			point_xy.push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x-1,pixel_for_now.y-1) = new_flood_val;
		}

		// x y +1 -1
		if (pixel_for_now.x < img_in.Width()-1 && pixel_for_now.y > 0
			&& img_in(pixel_for_now.x+1,pixel_for_now.y-1) == proto_val 
			&& (*img_out)(pixel_for_now.x+1,pixel_for_now.y-1) != new_flood_val)  
		{	
			pixel_new_frontier.x=pixel_for_now.x+1;
			pixel_new_frontier.y=pixel_for_now.y-1;
			point_xy.push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x+1,pixel_for_now.y-1) = new_flood_val;
		}

		// x y -1 +1
		if (pixel_for_now.x > 0 && pixel_for_now.y  < img_in.Height()-1
			&& img_in(pixel_for_now.x-1,pixel_for_now.y+1) == proto_val 
			&& (*img_out)(pixel_for_now.x-1,pixel_for_now.y+1) != new_flood_val)  
		{	
			pixel_new_frontier.x=pixel_for_now.x-1;
			pixel_new_frontier.y=pixel_for_now.y+1;
			point_xy.push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x-1,pixel_for_now.y+1) = new_flood_val;
		}



		//for 4neighbour
		if (pixel_for_now.x > 0 
			&& img_in(pixel_for_now.x-1,pixel_for_now.y) == proto_val 
			&& (*img_out)(pixel_for_now.x-1,pixel_for_now.y) != new_flood_val)  
		{	

			pixel_new_frontier.x=pixel_for_now.x-1;
			pixel_new_frontier.y=pixel_for_now.y;
			point_xy.push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x-1,pixel_for_now.y) = new_flood_val;
		}
		


		if (pixel_for_now.x < img_in.Width()-1 
			&& img_in(pixel_for_now.x+1,pixel_for_now.y) == proto_val 
			&& (*img_out)(pixel_for_now.x+1,pixel_for_now.y) != new_flood_val)  
		{	
			pixel_new_frontier.x=pixel_for_now.x+1;
			pixel_new_frontier.y=pixel_for_now.y;
			point_xy.push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x+1,pixel_for_now.y) = new_flood_val;
		}
		
		
		if (pixel_for_now.y > 0 
			&& img_in(pixel_for_now.x,pixel_for_now.y-1) == proto_val 
			&& (*img_out)(pixel_for_now.x,pixel_for_now.y-1) != new_flood_val)  
		{	

			pixel_new_frontier.x=pixel_for_now.x;
			pixel_new_frontier.y=pixel_for_now.y-1;
			point_xy.push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x,pixel_for_now.y-1) = new_flood_val;
			
		}
		

		if (pixel_for_now.y  < img_in.Height()-1 
			&&img_in(pixel_for_now.x,pixel_for_now.y+1) == proto_val 
			&& (*img_out)(pixel_for_now.x,pixel_for_now.y+1) != new_flood_val)  
		{	

			pixel_new_frontier.x=pixel_for_now.x;
			pixel_new_frontier.y=pixel_for_now.y+1;
			point_xy.push(pixel_new_frontier);
			(*img_out)(pixel_for_now.x,pixel_for_now.y+1) = new_flood_val;
			
		}
		

		
	
		


	}//while
}




	


int Chamfering_distance_compute(const ImgFloat &img_in, const int ratio_chamfer, ImgFloat* img_out)
{
	float offset_value=10;
	float maximun_pixel = offset_value * ratio_chamfer;

	(*img_out).Reset(img_in.Width(), img_in.Height());
	
	
	Set(img_out,maximun_pixel);


	//first filter
	for( int y=1; y< img_in.Height()-1; y++ ) 
	{
		for( int x=1; x< img_in.Width()-1; x++ ) 
		{	
			
			if (img_in(x, y)==0) 
			{
				
				float comparation_pixel = maximun_pixel;

				comparation_pixel = min( comparation_pixel, (*img_out)(x, y-1) + offset_value);
				comparation_pixel = min( comparation_pixel, (*img_out)(x-1, y) + offset_value);
				
				comparation_pixel = min( comparation_pixel, (*img_out)(x+1, y-1) + offset_value);
				comparation_pixel = min( comparation_pixel, (*img_out)(x-1, y-1) + offset_value);
				
				
				(*img_out)(x,y) = comparation_pixel;
			}

			else  (*img_out)(x, y) = 0;
      			
			
		}//for x
	}//for y





	//Second filter
	for( int y=img_in.Height()-2; y>0 ; y-- ) 
	{
		for( int x=img_in.Width()-2; x>0; x-- ) 
		{	
			
			if (img_in(x, y)==0) 
			{
				
				float comparation_pixel = (*img_out)(x, y);
				
				comparation_pixel = min( comparation_pixel, (*img_out)(x, y+1) + offset_value);
				comparation_pixel = min( comparation_pixel, (*img_out)(x+1, y) + offset_value);
				
				comparation_pixel = min( comparation_pixel, (*img_out)(x-1, y+1) + offset_value);
				comparation_pixel = min( comparation_pixel, (*img_out)(x+1, y+1) + offset_value);

				(*img_out)(x,y) = comparation_pixel;
			}
						
			else  (*img_out)(x, y) = 0;
			
		}//for x
	}//for y


	return 0;
}


int Chamfering_distance_compute(const ImgGray &img_in, const int ratio_chamfer, ImgGray* img_out)
{
	int offset_value=255/ratio_chamfer;
	int maximun_pixel = 255;

	(*img_out).Reset(img_in.Width(), img_in.Height());
	//(*img_out)=img_in;
	
	//float median_pixel_chamfer = (img_in.Width()+img_in.Height() )/2*0.1f/2;
	
	Set(img_out,maximun_pixel);


	//first filter
	for( int y=1; y< img_in.Height()-1; y++ ) 
	{
		for( int x=1; x< img_in.Width()-1; x++ ) 
		{	
			
			if (img_in(x, y)==0) 
			{
				//(*img_out)(x,y) = img_in(x,y)?0 :min(ininity, 1+d(x-1,y), 1+d(x,y-1));
				int comparation_pixel = maximun_pixel;

				comparation_pixel = min( comparation_pixel, (*img_out)(x, y-1) + offset_value);
				comparation_pixel = min( comparation_pixel, (*img_out)(x-1, y) + offset_value);

				comparation_pixel = min( comparation_pixel, (*img_out)(x+1, y-1) + offset_value);
				comparation_pixel = min( comparation_pixel, (*img_out)(x-1, y-1) + offset_value);
				
				
				(*img_out)(x,y) = comparation_pixel;
			}

			else  (*img_out)(x, y) = 0;
      			
			
		}//for x
	}//for y





	//Second filter
	for( int y=img_in.Height()-2; y>0 ; y-- ) 
	{
		for( int x=img_in.Width()-2; x>0; x-- ) 
		{	
			
			if (img_in(x, y)==0) 
			{
				//(*img_out)= img_in(x,y) ? 0 : min( d(x,y), 1+d(x+1,y), 1+d(x,y+1) );
				int comparation_pixel = (*img_out)(x, y);

				comparation_pixel = min( comparation_pixel, (*img_out)(x, y+1) + offset_value);
				comparation_pixel = min( comparation_pixel, (*img_out)(x+1, y) + offset_value);

				comparation_pixel = min( comparation_pixel, (*img_out)(x-1, y+1) + offset_value);
				comparation_pixel = min( comparation_pixel, (*img_out)(x+1, y+1) + offset_value);
				
				
				(*img_out)(x,y) = comparation_pixel;
			}
						
			else  (*img_out)(x, y) = 0;
			
		}//for x
	}//for y


	return 0;
}










int nonmarker_based_watershed_cal( const ImgGray &img_in, 
								  ImgInt* img_out, 
								  const int non_marker_based_val) 
{
	//initial
	(*img_out).Reset(img_in.Width(),img_in.Height());
	Set(img_out, -1);
	

	ImgInt img_in_int;

	Convert(img_in,&img_in_int);

	int next_gray_val=1;
	int boundaryoffset=2;


	//Gray level max number
	const int gray_level=256;

	//gray level   gradmag_pixel_list[gray level]
	//x  gradient_magnitude_pixel_list[](?, 0)
	//y  gradient_magnitude_pixel_list[](?, 1)

	ImgInt gradient_magnitude_pixel_list[gray_level];
	int element_number_for_gray[gray_level]={0};


	for( int count_num=0; count_num<gray_level; count_num++)
	{
		gradient_magnitude_pixel_list[count_num].Reset( img_in.Width() * img_in.Height() ,4);
		Set(&gradient_magnitude_pixel_list[count_num],-1);
	}
	
	for( int y=1; y< img_in.Height()-1; y++ ) 
	{
		for( int x=1; x< img_in.Width()-1; x++ ) 
		{			
			gradient_magnitude_pixel_list[img_in(x,y)](element_number_for_gray[img_in(x,y)], 1)=y;
			gradient_magnitude_pixel_list[img_in(x,y)](element_number_for_gray[img_in(x,y)], 0)=x;
			
			element_number_for_gray[img_in(x,y)]++;

		}//for x
	}//for y


	node pixel_new_frontier;

	queue<node> point_xy;



	//Watershed 
	for(int pixel_level_count_num=0; pixel_level_count_num<gray_level; pixel_level_count_num++)
	{

		for(int count_num=0; count_num<element_number_for_gray[pixel_level_count_num] ; count_num++)
		{
			//grow catchment basins
			
			
			
			pixel_new_frontier.x= gradient_magnitude_pixel_list[pixel_level_count_num](count_num, 0);
			pixel_new_frontier.y= gradient_magnitude_pixel_list[pixel_level_count_num](count_num, 1);
			
			int sign_for_add_fronter=0;

			if( pixel_new_frontier.y >1 && pixel_new_frontier.y< img_in.Height()-2)
			{
				if( (*img_out)(pixel_new_frontier.x,pixel_new_frontier.y+1)>0 )
				{
					(*img_out)(pixel_new_frontier.x,pixel_new_frontier.y)=(*img_out)(pixel_new_frontier.x,pixel_new_frontier.y+1);
					sign_for_add_fronter=1;
				}

				else if( (*img_out)(pixel_new_frontier.x,pixel_new_frontier.y-1)>0 )
				{
					(*img_out)(pixel_new_frontier.x,pixel_new_frontier.y)=(*img_out)(pixel_new_frontier.x,pixel_new_frontier.y-1);
					sign_for_add_fronter=1;
				}
				
			}
			


			if( pixel_new_frontier.x >1 && pixel_new_frontier.x< img_in.Width()-2)
			{
				if( (*img_out)(pixel_new_frontier.x+1,pixel_new_frontier.y)>0 )
				{
					(*img_out)(pixel_new_frontier.x,pixel_new_frontier.y)=(*img_out)(pixel_new_frontier.x+1,pixel_new_frontier.y);
					sign_for_add_fronter=1;
				}
				
				else if( (*img_out)(pixel_new_frontier.x-1,pixel_new_frontier.y)>0 )
				{
					(*img_out)(pixel_new_frontier.x,pixel_new_frontier.y)=(*img_out)(pixel_new_frontier.x-1,pixel_new_frontier.y);
					sign_for_add_fronter=1;
				}
			}
			
			if( 1==sign_for_add_fronter )
			{
				point_xy.push(pixel_new_frontier);
			}
		}//for ctn
	

		//for expanding
		inner_nonmarker_watershed_floodfill_4neighbour_wt_queue_offset( img_in_int, pixel_level_count_num,&point_xy ,img_out, boundaryoffset);

		if (pixel_level_count_num==0 || 1==non_marker_based_val )
		{
			for (int count_num=0; count_num<element_number_for_gray[pixel_level_count_num] ; count_num++)
			{
				int xx=0;
				int yy=0;
				yy= gradient_magnitude_pixel_list[pixel_level_count_num](count_num, 1);
				xx= gradient_magnitude_pixel_list[pixel_level_count_num](count_num, 0);
				

				if ((*img_out)(xx, yy) < 0)  
				{
					floodfill_8neighbour_wt_queue(img_in_int, xx, yy, next_gray_val++, img_out);
				}
			}
		}
	
	}//for pixel gray level

	return 0;
}


int marker_based_watershed_cal( const ImgGray &img_in,
							    const ImgInt &img_marker, 
								ImgInt* img_out, 
								const int marker_based, 
								const int offset_for_boundary) 
{
	//initial
	(*img_out).Reset(img_in.Width(),img_in.Height());
	Set(img_out, -1);
	


	//Gray level Precompute
	const int gray_level=256;

	//gray level   gradmag_pixel_list[gray level]
	//x  gradient_magnitude_pixel_list[](?, 0)
	//y  gradient_magnitude_pixel_list[](?, 1)
	int element_number_for_gray[gray_level]={0};
	ImgInt gradient_magnitude_pixel_list[gray_level];
	

	ImgInt img_in_int;
	Convert(img_in,&img_in_int);
	int next_gray_val=1;


	for( int count_num=0; count_num<gray_level; count_num++)
	{
		gradient_magnitude_pixel_list[count_num].Reset( img_in.Width() * img_in.Height() ,4);
		Set(&gradient_magnitude_pixel_list[count_num],-1);
	}
	
	for( int y=offset_for_boundary; y< img_in.Height()-offset_for_boundary; y++ ) 
	{
		for( int x=offset_for_boundary; x< img_in.Width()-offset_for_boundary; x++ ) 
		{		
			gradient_magnitude_pixel_list[img_in(x,y)](element_number_for_gray[img_in(x,y)], 1)=y;
			gradient_magnitude_pixel_list[img_in(x,y)](element_number_for_gray[img_in(x,y)], 0)=x;
			
			element_number_for_gray[img_in(x,y)]++;

		}//for x
	}//for y

	node pixel_new_frontier;

	queue<node> point_xy;

	//Watershed 
	for(int pixel_gray_count_num=0; pixel_gray_count_num<gray_level; pixel_gray_count_num++)
	{
		
		for(int count_num=0; count_num<element_number_for_gray[pixel_gray_count_num] ; count_num++)
		{
			
			//grow catchment basins
			pixel_new_frontier.y= gradient_magnitude_pixel_list[pixel_gray_count_num](count_num, 1);//y
			pixel_new_frontier.x= gradient_magnitude_pixel_list[pixel_gray_count_num](count_num, 0);//x
	
			int sign_for_add_fronter=0;

			if( pixel_new_frontier.y >offset_for_boundary && pixel_new_frontier.y< img_in.Height()-offset_for_boundary)
			{
				if( (*img_out)(pixel_new_frontier.x,pixel_new_frontier.y+1)>0 )
				{
					(*img_out)(pixel_new_frontier.x,pixel_new_frontier.y)=(*img_out)(pixel_new_frontier.x,pixel_new_frontier.y+1);
					sign_for_add_fronter=1;
				}
				
				else if( (*img_out)(pixel_new_frontier.x,pixel_new_frontier.y-1)>0 )
				{
					(*img_out)(pixel_new_frontier.x,pixel_new_frontier.y)=(*img_out)(pixel_new_frontier.x,pixel_new_frontier.y-1);
					sign_for_add_fronter=1;
				}
				
			}
			
			

			if( pixel_new_frontier.x >offset_for_boundary && pixel_new_frontier.x< img_in.Width()-offset_for_boundary)
			{
				
				if( (*img_out)(pixel_new_frontier.x+1,pixel_new_frontier.y)>0 )
				{
					(*img_out)(pixel_new_frontier.x,pixel_new_frontier.y)=(*img_out)(pixel_new_frontier.x+1,pixel_new_frontier.y);
					sign_for_add_fronter=1;
				}
							
				
				else if( (*img_out)(pixel_new_frontier.x-1,pixel_new_frontier.y)>0 )
				{
					(*img_out)(pixel_new_frontier.x,pixel_new_frontier.y)=(*img_out)(pixel_new_frontier.x-1,pixel_new_frontier.y);
					sign_for_add_fronter=1;
				}
				
				
			}
			
			if( 1==sign_for_add_fronter )
			{
				point_xy.push(pixel_new_frontier);
			}
		}//for ctn
	
		//for expandind
		inner_marker_watershed_floodfill_4neighbour_wt_queue_boundaryoffset( img_in_int, pixel_gray_count_num,&point_xy ,img_out, offset_for_boundary);

	

		if (pixel_gray_count_num==0)
		{
			for (int count_num=0; count_num<element_number_for_gray[pixel_gray_count_num] ; count_num++)
			{
				int x=0,y=0;
				x= gradient_magnitude_pixel_list[pixel_gray_count_num](count_num, 0);
				y= gradient_magnitude_pixel_list[pixel_gray_count_num](count_num, 1);

				if ( img_marker(x, y)==1 &&(*img_out)(x, y) < 0  )
				{
					
					floodfill_8neighbour_wt_queue(img_marker, x, y, next_gray_val++, img_out);
				}
			}
		}
	
	}//for pixel gray level


	return 0;
}








int edge_detection( const ImgInt &img_in, ImgInt* img_out) 
{
	(*img_out).Reset( img_in.Width(), img_in.Height() );
	Set(img_out,0);


	for( int y=1; y< img_in.Height()-1; y++ ) 
	{
		for( int x=1; x< img_in.Width()-1; x++ ) 
		{	
			int neigbour_diff_value_sign=0;
			//4 neigbours
			if( x>1 && img_in(x,y) != img_in(x-1,y) && (*img_out)(x-1,y)!=1 ) 
				neigbour_diff_value_sign=1;

			if( x<img_in.Width()-2 && img_in(x,y) != img_in(x+1,y) && (*img_out)(x+1,y)!=1 ) 
				neigbour_diff_value_sign=1;
			
			if( y>1 && img_in(x,y) != img_in(x,y-1) && (*img_out)(x,y-1)!=1 ) 
				neigbour_diff_value_sign=1;

			if( y<img_in.Height()-2 && img_in(x,y) != img_in(x,y+1) && (*img_out)(x,y+1)!=1 ) 
				neigbour_diff_value_sign=1;

			if( 1==neigbour_diff_value_sign )
				(*img_out)(x,y)=1;


		

		}//for x
	}//for y
	
	return 0;
}


int edge_detection( const ImgInt &img_in, ImgBgr* img_out, const Bgr &color) 
{
	(*img_out).Reset( img_in.Width(), img_in.Height() );
	Set(img_out,Bgr(0,0,0));


	for( int y=1; y< img_in.Height()-1; y++ ) 
	{
		for( int x=1; x< img_in.Width()-1; x++ ) 
		{	
			int neigbour_diff_value_sign=0;
			//4 neigbours
			if( x>1 && img_in(x,y) != img_in(x-1,y) && (*img_out)(x-1,y)!=color ) 
				neigbour_diff_value_sign=1;

			if( x<img_in.Width()-2 && img_in(x,y) != img_in(x+1,y) && (*img_out)(x+1,y)!=color ) 
				neigbour_diff_value_sign=1;
			
			if( y>1 && img_in(x,y) != img_in(x,y-1) && (*img_out)(x,y-1)!=color ) 
				neigbour_diff_value_sign=1;

			if( y<img_in.Height()-2 && img_in(x,y) != img_in(x,y+1) && (*img_out)(x,y+1)!=color ) 
				neigbour_diff_value_sign=1;

			if( 1==neigbour_diff_value_sign )
				(*img_out)(x,y)=color;


		

		}//for x
	}//for y
	
	return 0;
}








//Using Sobel kernel is faster
//void Gradient_Magnitude_Sobel(const ImgGray& img_in, ImgGray* img_out)
//{
//	//initial
//	(*img_out).Reset( img_in.Width(), img_in.Height() );
//	Set(img_out,0);
//	int offset=1;
//
//
//
//
//
//
//
//
//}












