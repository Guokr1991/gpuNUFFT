#include <limits.h>

#include "gtest/gtest.h"

#include "gridding_operator_factory.hpp"

#define epsilon 0.0001f

#define get3DC2lin(_x,_y,_z,_width) ((_x) + (_width) * ( (_y) + (_z) * (_width)))

TEST(TestGPUGriddingDeapo,KernelCall1Sector)
{
	int kernel_width = 5;
	//oversampling ratio
	float osr = 1.75;//DEFAULT_OVERSAMPLING_RATIO;

	//Image
	int im_width = 64;

	//Data
	int data_entries = 1;
    DType2* data = (DType2*) calloc(data_entries,sizeof(DType2)); //2* re + im
	data[0].x = 1;//Re
	data[0].y = 0;//Im

	//Coords
	//Scaled between -0.5 and 0.5
	//in triplets (x,y,z)
    DType* coords = (DType*) calloc(3*data_entries,sizeof(DType));//3* x,y,z
	coords[0] = 0; //should result in 7,7,7 center
	coords[1] = 0;
	coords[2] = 0;

	//sectors of data, count and start indices
	int sector_width = 8;
	
	GriddingND::Array<DType> kSpaceData;
    kSpaceData.data = coords;
    kSpaceData.dim.length = data_entries;

	GriddingND::Dimensions imgDims;
	imgDims.width = im_width;
	imgDims.height = im_width;
	imgDims.depth = im_width;

    GriddingND::GriddingOperator *griddingOp = GriddingND::GriddingOperatorFactory::getInstance().createGriddingOperator(kSpaceData,kernel_width,sector_width,osr,imgDims);

	GriddingND::Array<DType2> dataArray;
	dataArray.data = data;
	dataArray.dim.length = data_entries;

	GriddingND::Array<CufftType> gdataArray;
	
	gdataArray = griddingOp->performGriddingAdj(dataArray,GriddingND::DEAPODIZATION);
	//Output Grid
	CufftType* gdata = gdataArray.data;

	printf("test %f \n",gdata[4].x);

	/*for (int y = 0; y < im_width; y++)
	{
		for (int x = 0; x < im_width; x++)
			printf(" %.3f ", gdata[get3DC2lin(x,y,16,im_width)].x,epsilon);
		printf(" \n ");
	}*/

	int index = get3DC2lin(5,5,5,im_width);
	printf("index to test %d\n",index);
	EXPECT_NEAR(0.01294,gdata[get3DC2lin(10,0,16,im_width)].x,epsilon);
	
	free(data);
	free(coords);
	free(gdata);

	delete griddingOp;
}

TEST(TestGPUGriddingDeapo,KernelCall1Sector2Coils)
{
	int kernel_width = 5;
	//oversampling ratio
	float osr = 2.0f;//DEFAULT_OVERSAMPLING_RATIO;

	//Image
	int im_width = 32;

	//Data
	int data_entries = 1;
	int n_coils = 2;

    DType2* data = (DType2*) calloc(data_entries*n_coils,sizeof(DType2)); //2* re + im
	data[0].x = 1;//Re
	data[0].y = 0;//Im
	data[1].x = 1;//Re
	data[1].y = 0;//Im
	
	//Coords, same per coil
	//Scaled between -0.5 and 0.5
	//in triplets (x,y,z)
    DType* coords = (DType*) calloc(3*data_entries,sizeof(DType));//3* x,y,z
	coords[0] = 0; 
	coords[1] = 0;
	coords[2] = 0;

	//sectors of data, count and start indices
	int sector_width = 8;
	
	GriddingND::Array<DType> kSpaceData;
    kSpaceData.data = coords;
    kSpaceData.dim.length = data_entries;

	GriddingND::Dimensions imgDims;
	imgDims.width = im_width;
	imgDims.height = im_width;
	imgDims.depth = im_width;

    GriddingND::GriddingOperator *griddingOp = GriddingND::GriddingOperatorFactory::getInstance().createGriddingOperator(kSpaceData,kernel_width,sector_width,osr,imgDims);

	GriddingND::Array<DType2> dataArray;
	dataArray.data = data;
	dataArray.dim.length = data_entries;
	dataArray.dim.channels = n_coils;

	GriddingND::Array<CufftType> gdataArray;
	
	gdataArray = griddingOp->performGriddingAdj(dataArray,GriddingND::DEAPODIZATION);
	//Output Grid
	CufftType* gdata = gdataArray.data;
	
	printf("test %f \n",gdata[4].x);
	int index = get3DC2lin(5,5,5,im_width);
	printf("index to test %d\n",index);

	int coil_offset = 1 * griddingOp->getImageDims().count();
	printf("grid_size: %d\n",griddingOp->getImageDims().count());
	
	/*for (int j=0; j<im_width; j++)
	{
		for (int i=0; i<im_width; i++)
			printf("A: %.2f B: %.2f",gdata[get3DC2lin(i,j,16,im_width)].x,gdata[coil_offset+get3DC2lin(i,j,16,im_width)].x);
		printf("\n");
	}*/
	EXPECT_NEAR(gdata[get3DC2lin(0,0,16,im_width)].x,0.033939f,epsilon);
	//EXPECT_NEAR(gdata[coil_offset + get3DC2lin(0,0,16,im_width)].x,0.00928f,epsilon);
	EXPECT_NEAR(gdata[get3DC2lin(10,0,16,im_width)].x,gdata[coil_offset + get3DC2lin(10,0,16,im_width)].x,epsilon);
	EXPECT_NEAR(gdata[get3DC2lin(0,5,16,im_width)].x,gdata[coil_offset + get3DC2lin(0,5,16,im_width)].x,epsilon);
	EXPECT_NEAR(gdata[get3DC2lin(4,4,16,im_width)].x,gdata[coil_offset + get3DC2lin(4,4,16,im_width)].x,epsilon);
	EXPECT_NEAR(gdata[get3DC2lin(15,14,16,im_width)].x,gdata[coil_offset + get3DC2lin(15,14,16,im_width)].x,epsilon);
	
	free(data);
	free(coords);
	free(gdata);

	delete griddingOp;

	EXPECT_EQ(1, 1);
}
