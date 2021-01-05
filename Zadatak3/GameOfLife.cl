// TODO: Add OpenCL kernel code here.
//#pragma OPENCL EXTENSION cl_khr_fp64 : enable

//inline bool willLive(__global int *inMatrix, int aliveNeighbours){                                       
 //   return (inMatrix[(row)*n + col]) && (aliveNeighbours==3 || aliveNeighbours==2) ||                                      
 //          (!(inMatrix[(row)*n + col]) && aliveNeighbours==3);                                                         
//} 

__kernel void gameOfLife(__global unsigned char* inMatrix, __global unsigned char* outMatrix, const int n){
	
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

//	barrier(CLK_GLOBAL_MEM_FENCE);
	//if (isAlive && (aliveNeighbours == 3 || aliveNeighbours == 2) || !isAlive && aliveNeighbours == 3) {
	//		outMatrix[(row)*n + col] = 0;
	//	}
//	else{
//		outMatrix[(row)*n + col] = 1;
//	}
if ((isAlive && aliveNeighbours == 2) || (aliveNeighbours == 3))
	{
		outMatrix[(row)*n + (col)] = 0;
	}
	else
	{
		outMatrix[(row)*n + (col)] = 1;
	}
	//printf("bio sam ovdje1");
	//__global unsigned char* temp=inMatrix;
	//inMatrix = outMatrix;
	//outMatrix=temp;
//	printf("bio sam ovdje");
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
	printf("%d ", (j - startY)*imageWidth + (i - startX));
}

private unsigned char ToByte(bool b[8])
{
    unsigned char c = 0;
    for (int i=0; i < 8; ++i)
        if (b[i])
            c |= 1 << i;
    return c;
}

private void FromByte(unsigned char c, bool b[8])
{
    for (int i=0; i < 8; ++i)
        b[i] = (c & (1<<i)) != 0;
}

