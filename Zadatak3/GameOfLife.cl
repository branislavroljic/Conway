
int index(int i, int j, int width)
{
	return j*width+i;
}
struct Pixel
{
	unsigned char r, g, b;
};


bool isVerticalBlinker(__global unsigned char* matrix, unsigned int i, unsigned int j, const unsigned int width){
	return matrix[index(i, j, width)] == 1 &&
			matrix[index(i, j-1, width)] == 1 &&
			matrix[index(i, j+1, width)] == 1 &&
			//svi zivi
			matrix[index(i, j - 2, width)] == 0 &&
			matrix[index(i, j + 2, width)] == 0 &&
			matrix[index(i - 1, j - 2, width)] == 0 &&
			matrix[index(i + 1, j - 2, width)] == 0 &&
			matrix[index(i - 1, j + 2, width)] == 0 &&
			matrix[index(i + 1, j + 2, width)] == 0 &&
			matrix[index(i - 1, j - 1, width)] == 0 &&
			matrix[index(i - 1, j , width)] == 0 &&
			matrix[index(i - 1, j + 1 , width)] == 0 &&
			matrix[index(i + 1, j - 1, width)] == 0 &&
			matrix[index(i + 1, j , width)] == 0 &&
			matrix[index(i + 1, j + 1 , width)] == 0 ;
			
}

bool isHorizontalBlinker(__global unsigned char* matrix, unsigned int i, unsigned int j, const unsigned int width){
	return matrix[index(i, j, width)] == 1 &&
			matrix[index(i - 1, j, width)] == 1 &&
			matrix[index(i + 1, j, width)] == 1 &&
			//svi zivi
			matrix[index(i - 2, j, width)] == 0 &&
			matrix[index(i + 2, j, width)] == 0 &&
			matrix[index(i - 2, j - 1, width)] == 0 &&
			matrix[index(i - 2, j + 1, width)] == 0 &&
			matrix[index(i + 2, j - 1, width)] == 0 &&
			matrix[index(i + 2, j+1, width)] == 0 &&
			matrix[index(i - 1, j - 1, width)] == 0 &&
			matrix[index(i, j - 1, width)] == 0 &&
			matrix[index(i + 1, j - 1 , width)] == 0 &&
			matrix[index(i - 1, j + 1, width)] == 0 &&
			matrix[index(i, j + 1, width)] == 0 &&
			matrix[index(i + 1, j + 1 , width)] == 0 ;
			
}

//detekcija blinkera(horizontal i vertical)
//da bi se detektovao vertikalni blinker( u outMatrix koja predstavlja trenutnu iteraciju) , mora prethoda iteracija(inMatrix) sadrzati horisontalni blinker na istom mjestu i obrnuto
__kernel void detector (__global unsigned char* inMatrix, __global unsigned char* outMatrix, __global struct Pixel* RGBSegment, 
					const unsigned int matrixWidth, const unsigned int startCol, const unsigned int offset, const unsigned int iterationOffset ) {
		
		
	int n = matrixWidth;
	int col = get_global_id(0) + iterationOffset;
	int row = get_global_id(1);

	if(row == 0 || row == n - 1 || col == 0 || col == n - 1){
		return;
	}
	if(col < startCol || col >= startCol + offset){
		return;
	}
	
	if(outMatrix[row*n + col] == 1 && isVerticalBlinker(outMatrix, col, row, matrixWidth)
		&& isHorizontalBlinker(inMatrix, col, row, matrixWidth)){
		RGBSegment[row*n + col].g = 0;
		RGBSegment[(row - 1)*n + col].g = 0;
		RGBSegment[(row + 1)*n + col].g = 0;
		return;
	}
	
	if(outMatrix[row*n + col] == 1  && isHorizontalBlinker(outMatrix, col, row, matrixWidth)
	&& isVerticalBlinker(inMatrix, col, row, matrixWidth)){
		RGBSegment[row*n + col].g = 0;
		RGBSegment[row*n + (col  - 1)].g = 0;
		RGBSegment[row*n + (col + 1)].g = 0;
	}
}

//pomocna funkcija koja sluzi za odluku da li je celija koju ispitijuemo ziva ili mrtva u narednoj iteraciji
void setCellValue(__global unsigned char* inMatrix, __global unsigned char* outMatrix, const unsigned int n,
 const unsigned int startCol, const unsigned int offset, const unsigned int col, const unsigned int row){
		
	if(row == 0 || row == n - 1 || col == 0 || col == n - 1){
		return;
	}
	if(col < startCol || col >= startCol + offset){
		return;
	}
	
	int aliveNeighbours = 0;
	int isAlive = (inMatrix[(row)*n + col]);

	
	aliveNeighbours += (inMatrix[(row - 1)*n + col - 1] ? 1 : 0);
	aliveNeighbours += (inMatrix[(row - 1)*n + col] ? 1 : 0);
	aliveNeighbours += (inMatrix[(row - 1)*n + col + 1] ? 1 : 0);
	aliveNeighbours += (inMatrix[(row)*n + col - 1] ? 1 : 0);
	aliveNeighbours += (inMatrix[(row)*n + col + 1] ? 1 : 0);
	aliveNeighbours += (inMatrix[(row + 1)*n + col - 1] ? 1 : 0);
	aliveNeighbours += (inMatrix[(row + 1)*n + col] ? 1 : 0);
	aliveNeighbours += (inMatrix[(row + 1)*n + col + 1] ? 1 : 0);

//printf("aliveNeighbours %d za %d %d\n", aliveNeighbours, row, col);
if ((isAlive && aliveNeighbours == 2) || (aliveNeighbours == 3))
	{
		outMatrix[(row)*n + (col)] = 1;
	}
	else
	{
		outMatrix[(row)*n + (col)] = 0;
	}
}

//izracunavanje naredne iteracije i kreiranje RGB slike od date matrice
__kernel void gameOfLifeOscilator(__global unsigned char* inMatrix, __global unsigned char* outMatrix, __global struct Pixel* RGBSegment, 
					const unsigned int n, const unsigned int startCol, const unsigned int offset, const unsigned int iterationOffset ){
	
	
	int col = get_global_id(0) + iterationOffset;
	int row = get_global_id(1);
	setCellValue(inMatrix, outMatrix, n, startCol, offset, col, row);
	
	if(row == 0 || row == n - 1 || col == 0 || col == n - 1){
		return;
	}
	if(col < startCol || col >= startCol + offset){
		return;
	}
	
	if(outMatrix[row*n + col] == 0){
		RGBSegment[row*n + col].r = 0;
		RGBSegment[row*n + col].g = 0;
		RGBSegment[row*n + col].b = 0;
	}else{
		RGBSegment[row*n + col].r = 255;
		RGBSegment[row*n + col].g = 255;
		RGBSegment[row*n + col].b = 255;
	}
}

//izracunavanje naredne iteracije
// +iterationOffset se izracunava kao offset*trenutnaIteracija, na ovaj nacin omogucavam da se u vise pokretanja kernela obradi jedna slika
//prakticno sam izdjelio matricu po kolonama sirine offset
//moglo se sve uraditi u jednom poretanju kernela, ali profesor je trazio da omogucimo vise pokretanja kernela
__kernel void gameOfLife(__global unsigned char* inMatrix, __global unsigned char* outMatrix, const unsigned int n, const unsigned int startCol, const unsigned int offset, const unsigned int iterationOffset){
	
	int col = get_global_id(0) + iterationOffset;
	int row = get_global_id(1);
	setCellValue(inMatrix, outMatrix, n, startCol, offset, col, row);
}

//dohvaranje podsegmenta
//x0 i y0 su pocetne koordinate, a width i height su sirina i visina podsegemnta
__kernel void getSubsegment(__global unsigned char* matrix, __global unsigned char* subsegment,
							const unsigned int matrixWidth, const unsigned int x0, const unsigned int y0, const unsigned int width, const unsigned int height){
	int col = get_global_id(0);
	int row = get_global_id(1);
	
	if(col < x0 || col >= x0 + width || row < y0 || row >= y0 + height){
		return;	
	}
	
	subsegment[(row - y0)*width + (col - x0)] = matrix[row*matrixWidth + col];
}

//inicijlizacija matrice, svi pixeli su crni
__kernel void initMatrix(__global unsigned char* inMatrix, const unsigned int width, const unsigned int height, const unsigned int offset, const unsigned int iterationOffset, const unsigned int startCol){

	int i = get_global_id(0) + iterationOffset;
	int j = get_global_id(1);
	
	if(i >= width || j >= height) return;
	
	if(i < startCol || i >= startCol + offset){
		return;
	}
	inMatrix[j*width + i] = 0;

}

//ako korisnik zeli rucno da inicijlizuje matricu
__kernel void userInitMatrix(__global unsigned char* inMatrix, const unsigned int iterationOffset, int xInput, int yInput, const unsigned int width){

	int i = get_global_id(0) + iterationOffset;
	int j = get_global_id(1);
	
	if ( i != xInput || j != yInput){
		return;
	}
	
	inMatrix[j*width + i] = 1;

}
//inicijlizacija matrice slikom
__kernel void initMatrixWithImage(__global unsigned char* inMatrix,__global unsigned char* image, 
									const unsigned int startX, const unsigned int startY, const unsigned int widthBoundary,
									const unsigned int heightBoundary, const unsigned int matrixWidth, const unsigned int imageWidth){

	int i = get_global_id(0);
	int j = get_global_id(1);

	if(i >= widthBoundary || i < startX || j >= heightBoundary || j < startY) return;

	inMatrix[j*matrixWidth + i] = image[(j - startY)*imageWidth + (i - startX)];
}

