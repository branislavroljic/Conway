// TODO: Add OpenCL kernel code here.
//#pragma OPENCL EXTENSION cl_khr_fp64 : enable

//inline bool willLive(__global int *inMatrix, int aliveNeighbours){                                       
 //   return (inMatrix[(row)*n + col]) && (aliveNeighbours==3 || aliveNeighbours==2) ||                                      
 //          (!(inMatrix[(row)*n + col]) && aliveNeighbours==3);                                                         
//} 

int index(int i, int j, int width)
{
	return j*width+i;
}
struct Pixel
{
	unsigned char r, g, b;
};


bool isVerticalBlinker(__global unsigned char* matrix, unsigned int i, unsigned int j, const unsigned int width){
	return matrix[index(i, j, width)] == 0 &&
			matrix[index(i, j-1, width)] == 0 &&
			matrix[index(i, j+1, width)] == 0 &&
			//svi zivi
			matrix[index(i, j - 2, width)] == 1 &&
			matrix[index(i, j + 2, width)] == 1 &&
			matrix[index(i - 1, j - 2, width)] == 1 &&
			matrix[index(i + 1, j - 2, width)] == 1 &&
			matrix[index(i - 1, j + 2, width)] == 1 &&
			matrix[index(i + 1, j + 2, width)] == 1 &&
			matrix[index(i - 1, j - 1, width)] == 1 &&
			matrix[index(i - 1, j , width)] == 1 &&
			matrix[index(i - 1, j + 1 , width)] == 1 &&
			matrix[index(i + 1, j - 1, width)] == 1 &&
			matrix[index(i + 1, j , width)] == 1 &&
			matrix[index(i + 1, j + 1 , width)] == 1 ;
			
}


__kernel void detector (__global unsigned char* outMatrix, __global struct Pixel* RGBSegment, const unsigned int startX,
												const unsigned int startY, const unsigned int widthBoundary,
												const unsigned int heightBoundary, const unsigned int matrixWidth, const unsigned int imageWidth) {
		
	int col = get_global_id(0);
	int row = get_global_id(1);

	if(col < startX || col >= widthBoundary || row < startY || row >= heightBoundary)
		return;
	
	if(outMatrix[row*matrixWidth + col] == 0 && (row - startY > 1) && isVerticalBlinker(outMatrix, col, row, matrixWidth)){
		RGBSegment[(row - startY)*imageWidth + (col - startX)].g = 255;
		RGBSegment[(row - startY - 1)*imageWidth + (col - startX)].g = 255;
		RGBSegment[(row - startY + 1)*imageWidth + (col - startX)].g = 255;
	}
}


__kernel void gameOfLifeOscilator(__global unsigned char* inMatrix, __global unsigned char* outMatrix, __global struct Pixel* RGBSegment, const unsigned int startX, const unsigned int startY, const unsigned int widthBoundary,
									const unsigned int heightBoundary, const unsigned int matrixWidth, const unsigned int imageWidth ){
	
	int col = get_global_id(0);
	int row = get_global_id(1);
	int n = matrixWidth;

	if(row == 0 || row == n - 1 || col == 0 || col == n - 1){
		return;
	}


	int aliveNeighbours = 0;
	int isAlive = !(inMatrix[(row)*n + col]);

	//mat[(row)*n + col] = 128 * isAlive;
	
	aliveNeighbours += !(inMatrix[(row - 1)*n + col - 1]);
	aliveNeighbours += !(inMatrix[(row - 1)*n + col]);
	aliveNeighbours += !(inMatrix[(row - 1)*n + col + 1]);
	aliveNeighbours += !(inMatrix[(row)*n + col - 1]);
	aliveNeighbours += !(inMatrix[(row)*n + col + 1]);
	aliveNeighbours += !(inMatrix[(row + 1)*n + col - 1]);
	aliveNeighbours += !(inMatrix[(row + 1)*n + col]);
	aliveNeighbours += !(inMatrix[(row + 1)*n + col + 1]);

		if ((isAlive && aliveNeighbours == 2) || (aliveNeighbours == 3))
		{
			outMatrix[(row)*n + (col)] = 0;
		}
		else
		{
			outMatrix[(row)*n + (col)] = 1;
		}
		
	
	if(col < startX || col >= widthBoundary || row < startY || row >= heightBoundary)
		return;
	
	if(outMatrix[row*n + col] == 0){
		RGBSegment[(row - startY)*imageWidth + (col - startX)].r = 0;
		RGBSegment[(row - startY)*imageWidth + (col - startX)].g = 0;
		RGBSegment[(row - startY)*imageWidth + (col - startX)].b = 0;
	}else{
		RGBSegment[(row - startY)*imageWidth + (col - startX)].r = 255;
		RGBSegment[(row - startY)*imageWidth + (col - startX)].g = 255;
		RGBSegment[(row - startY)*imageWidth + (col - startX)].b = 255;
	}
		
	//if( isVerticalBlinker(outMatrix, col, row, n)){
	//	writeVerticalBlinker(outMatrix, col, row, n);
		//printf("nasao-------------------------------------------------------");
	//}
	//else{
	//printf("NIJE NASAOOOo-------------------------------------------------------");
//	}
	//
}
__kernel void gameOfLife(__global unsigned char* inMatrix, __global unsigned char* outMatrix, const unsigned int n){
	
	int col = get_global_id(0);
	int row = get_global_id(1);

	if(row == 0 || row == n - 1 || col == 0 || col == n - 1){
		return;
	}

	int aliveNeighbours = 0;
	int isAlive = !(inMatrix[(row)*n + col]);

	//mat[(row)*n + col] = 128 * isAlive;
	
	aliveNeighbours += !(inMatrix[(row - 1)*n + col - 1]);
	aliveNeighbours += !(inMatrix[(row - 1)*n + col]);
	aliveNeighbours += !(inMatrix[(row - 1)*n + col + 1]);
	aliveNeighbours += !(inMatrix[(row)*n + col - 1]);
	aliveNeighbours += !(inMatrix[(row)*n + col + 1]);
	aliveNeighbours += !(inMatrix[(row + 1)*n + col - 1]);
	aliveNeighbours += !(inMatrix[(row + 1)*n + col]);
	aliveNeighbours += !(inMatrix[(row + 1)*n + col + 1]);

if ((isAlive && aliveNeighbours == 2) || (aliveNeighbours == 3))
	{
		outMatrix[(row)*n + (col)] = 0;
	}
	else
	{
		outMatrix[(row)*n + (col)] = 1;
	}
	//if(input == 0)
	//	return;
//	barrier(CLK_GLOBAL_MEM_FENCE);
	//nadji obrazac
	
	
	//if (isAlive && (aliveNeighbours == 3 || aliveNeighbours == 2) || !isAlive && aliveNeighbours == 3) {
	//		outMatrix[(row)*n + col] = 0;
	//	}
//	else{
//		outMatrix[(row)*n + col] = 1;
//	}

	//printf("bio sam ovdje1");
//	__global unsigned char* temp=inMatrix;
//	inMatrix = outMatrix;
	//outMatrix=temp;
//	printf("bio sam ovdje");
}

__kernel void getSubsegment(__global unsigned char* matrix, __global unsigned char* subsegment, const unsigned int matrixWidth, const unsigned int x0, const unsigned int y0, const unsigned int width, const unsigned int height){
	int col = get_global_id(0);
	int row = get_global_id(1);
	
	if(row < x0 || row >= x0 + width || col < y0 || col >= y0 + height){
		return;	
	}
	
	subsegment[(row - y0)*width + (col - x0)] = matrix[row*matrixWidth + col];
}

__kernel void initMatrix(__global unsigned char* inMatrix, const unsigned int width, const unsigned int height){

	int i = get_global_id(0);
	int j = get_global_id(1);

	if(i >= width || j >= height) return;

	inMatrix[j*width + i] = 1;

}
__kernel void initMatrixWithImage(__global unsigned char* inMatrix,__global unsigned char* image, 
									const unsigned int startX, const unsigned int startY, const unsigned int widthBoundary,
									const unsigned int heightBoundary, const unsigned int matrixWidth, const unsigned int imageWidth){

	int i = get_global_id(0);
	int j = get_global_id(1);

	if(i >= widthBoundary || i < startX || j >= heightBoundary || j < startY) return;

	inMatrix[j*matrixWidth + i] = image[(j - startY)*imageWidth + (i - startX)];
//	printf("%d ", (j - startY)*imageWidth + (i - startX));
}

