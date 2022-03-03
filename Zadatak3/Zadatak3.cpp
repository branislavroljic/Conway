#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <CL/cl.h>
#include <string>
#include <iostream>
#include <set>
#pragma warning (disable : 6031)
#pragma warning (disable : 6011)

const int LOCAL_SIZE = 16;
const int MAX_KERNEL_NUM = 10;

const char* TranslateOpenCLError(cl_int errorCode)
{
	switch (errorCode)
	{
	case CL_SUCCESS:                            return "CL_SUCCESS";
	case CL_DEVICE_NOT_FOUND:                   return "CL_DEVICE_NOT_FOUND";
	case CL_DEVICE_NOT_AVAILABLE:               return "CL_DEVICE_NOT_AVAILABLE";
	case CL_COMPILER_NOT_AVAILABLE:             return "CL_COMPILER_NOT_AVAILABLE";
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case CL_OUT_OF_RESOURCES:                   return "CL_OUT_OF_RESOURCES";
	case CL_OUT_OF_HOST_MEMORY:                 return "CL_OUT_OF_HOST_MEMORY";
	case CL_PROFILING_INFO_NOT_AVAILABLE:       return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case CL_MEM_COPY_OVERLAP:                   return "CL_MEM_COPY_OVERLAP";
	case CL_IMAGE_FORMAT_MISMATCH:              return "CL_IMAGE_FORMAT_MISMATCH";
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case CL_BUILD_PROGRAM_FAILURE:              return "CL_BUILD_PROGRAM_FAILURE";
	case CL_MAP_FAILURE:                        return "CL_MAP_FAILURE";
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:       return "CL_MISALIGNED_SUB_BUFFER_OFFSET";                          //-13
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";   //-14
	case CL_COMPILE_PROGRAM_FAILURE:            return "CL_COMPILE_PROGRAM_FAILURE";                               //-15
	case CL_LINKER_NOT_AVAILABLE:               return "CL_LINKER_NOT_AVAILABLE";                                  //-16
	case CL_LINK_PROGRAM_FAILURE:               return "CL_LINK_PROGRAM_FAILURE";                                  //-17
	case CL_DEVICE_PARTITION_FAILED:            return "CL_DEVICE_PARTITION_FAILED";                               //-18
	case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:      return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";                         //-19
	case CL_INVALID_VALUE:                      return "CL_INVALID_VALUE";
	case CL_INVALID_DEVICE_TYPE:                return "CL_INVALID_DEVICE_TYPE";
	case CL_INVALID_PLATFORM:                   return "CL_INVALID_PLATFORM";
	case CL_INVALID_DEVICE:                     return "CL_INVALID_DEVICE";
	case CL_INVALID_CONTEXT:                    return "CL_INVALID_CONTEXT";
	case CL_INVALID_QUEUE_PROPERTIES:           return "CL_INVALID_QUEUE_PROPERTIES";
	case CL_INVALID_COMMAND_QUEUE:              return "CL_INVALID_COMMAND_QUEUE";
	case CL_INVALID_HOST_PTR:                   return "CL_INVALID_HOST_PTR";
	case CL_INVALID_MEM_OBJECT:                 return "CL_INVALID_MEM_OBJECT";
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case CL_INVALID_IMAGE_SIZE:                 return "CL_INVALID_IMAGE_SIZE";
	case CL_INVALID_SAMPLER:                    return "CL_INVALID_SAMPLER";
	case CL_INVALID_BINARY:                     return "CL_INVALID_BINARY";
	case CL_INVALID_BUILD_OPTIONS:              return "CL_INVALID_BUILD_OPTIONS";
	case CL_INVALID_PROGRAM:                    return "CL_INVALID_PROGRAM";
	case CL_INVALID_PROGRAM_EXECUTABLE:         return "CL_INVALID_PROGRAM_EXECUTABLE";
	case CL_INVALID_KERNEL_NAME:                return "CL_INVALID_KERNEL_NAME";
	case CL_INVALID_KERNEL_DEFINITION:          return "CL_INVALID_KERNEL_DEFINITION";
	case CL_INVALID_KERNEL:                     return "CL_INVALID_KERNEL";
	case CL_INVALID_ARG_INDEX:                  return "CL_INVALID_ARG_INDEX";
	case CL_INVALID_ARG_VALUE:                  return "CL_INVALID_ARG_VALUE";
	case CL_INVALID_ARG_SIZE:                   return "CL_INVALID_ARG_SIZE";
	case CL_INVALID_KERNEL_ARGS:                return "CL_INVALID_KERNEL_ARGS";
	case CL_INVALID_WORK_DIMENSION:             return "CL_INVALID_WORK_DIMENSION";
	case CL_INVALID_WORK_GROUP_SIZE:            return "CL_INVALID_WORK_GROUP_SIZE";
	case CL_INVALID_WORK_ITEM_SIZE:             return "CL_INVALID_WORK_ITEM_SIZE";
	case CL_INVALID_GLOBAL_OFFSET:              return "CL_INVALID_GLOBAL_OFFSET";
	case CL_INVALID_EVENT_WAIT_LIST:            return "CL_INVALID_EVENT_WAIT_LIST";
	case CL_INVALID_EVENT:                      return "CL_INVALID_EVENT";
	case CL_INVALID_OPERATION:                  return "CL_INVALID_OPERATION";
	case CL_INVALID_GL_OBJECT:                  return "CL_INVALID_GL_OBJECT";
	case CL_INVALID_BUFFER_SIZE:                return "CL_INVALID_BUFFER_SIZE";
	case CL_INVALID_MIP_LEVEL:                  return "CL_INVALID_MIP_LEVEL";
	case CL_INVALID_GLOBAL_WORK_SIZE:           return "CL_INVALID_GLOBAL_WORK_SIZE";                           //-63
	case CL_INVALID_PROPERTY:                   return "CL_INVALID_PROPERTY";                                   //-64
	case CL_INVALID_IMAGE_DESCRIPTOR:           return "CL_INVALID_IMAGE_DESCRIPTOR";                           //-65
	case CL_INVALID_COMPILER_OPTIONS:           return "CL_INVALID_COMPILER_OPTIONS";                           //-66
	case CL_INVALID_LINKER_OPTIONS:             return "CL_INVALID_LINKER_OPTIONS";                             //-67
	case CL_INVALID_DEVICE_PARTITION_COUNT:     return "CL_INVALID_DEVICE_PARTITION_COUNT";                     //-68
																												//    case CL_INVALID_PIPE_SIZE:                  return "CL_INVALID_PIPE_SIZE";                                  //-69
																												//    case CL_INVALID_DEVICE_QUEUE:               return "CL_INVALID_DEVICE_QUEUE";                               //-70    

	default:
		return "UNKNOWN ERROR CODE";
	}
}

static char* readKernelSource(const char* filename) {

	char* kernelSource = nullptr;
	long length;
	FILE* file = fopen(filename, "r");
	if (file) {
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);
		kernelSource = (char*)calloc(length, sizeof(char));
		if (kernelSource) {
			fread(kernelSource, sizeof(char), length, file);
		}
		fclose(file);
	}
	return kernelSource;
}

//funkcija za obradu unosa, korisnik moze unijeti slova koja su dio baseString parametra
//kada korisnik unese jedno od ponudjenih slova, unos predstavlja povratnu vrijenost
char getCharacterWithEscape(const std::string& baseString)
{
	const int ASCII_ESCAPE = 27;
	std::set<char> baseSet;
	for (char it : baseString)
	{
		baseSet.insert(it);
	}
	char c;
	do
	{
		c = getchar();
	} while (!baseSet.count(c) && c != ASCII_ESCAPE);

	return c;
}


struct Pixel {
	unsigned char r, g, b;
};

//upis slike sa .pgm ekstenzijom u fajl
static void writePGMImage(const char* filename, const unsigned char* array, const int width, const int height) {

	FILE* file = fopen(filename, "wb");
	fprintf(file, "P5\n%d %d\n255\n", width, height);
	fwrite(array, sizeof(unsigned char), (size_t)width * (size_t)height, file);
	fclose(file);
}

//upis slike sa .ppm ekstenzijom u fajl
void writePPMImage(const char* filename, const Pixel* array, const int width, const int height)
{
	FILE* fp = fopen(filename, "wb"); /* b - binary mode */
	fprintf(fp, "P6\n%d %d\n255\n", width, height);
	fwrite(array, sizeof(Pixel), (size_t)width * (size_t)height, fp);
	fclose(fp);
}

//citanje slike sa .pgm ekstenzijom iz fajla
static void readPGMImage(const char* filename, unsigned char*& array, int& width, int& height) {
	FILE* file = fopen(filename, "rb");
	if (file == NULL) {
		throw "file not present";
	}

	if (!fscanf(file, "P5\n%d %d\n255\n", &width, &height)) {
		throw "error";
	}

	unsigned char* image = nullptr;
	image = (unsigned char*)calloc((size_t)width * height, sizeof(unsigned char));
	if (image == 0) {
		return;
	}
	fread(image, sizeof(unsigned char), (size_t)width * (size_t)height, file);
	fclose(file);
	array = image;
}

//inicijalizovanje svijeta. Pri pokretanju programa, matrica se inicijalizuje tako da su svi pixeli bijele boje.
unsigned char* initMatrix(cl_program program, cl_kernel kernel, cl_context context, cl_command_queue queue, size_t* globalSize, size_t* localSize, int kernelNum);

//inizijalizacija svijeta sa slikom, korisnik ce unijeti koordinate na koje zeli smjestit sliku, i slika ce biti smjestena unutar svijeta
unsigned char* initMatrixWithImage(size_t*, size_t*);

//konverzija binarne matrice u matricu u kojoj je bijeli pixel setovan na 255, a crni na 0
unsigned char* convertBinaryToBlackAndWhiteMatrix(unsigned char* matrix, unsigned int length) {

	unsigned char* retMatrix = (unsigned char*)malloc(length);
	for (size_t i = 0; i < length; i++)
	{
		if (matrix[i] == 0) {
			retMatrix[i] = 0;
		}
		else {
			retMatrix[i] = 255;
		}
	}
	return retMatrix;
}

int index(int i, int j, int width)
{
	return j * width + i;
}


int main() {
	int width = -1;
	int height = -1;

	cl_mem d_InMatrix;
	cl_mem d_OutMatrix;

	cl_platform_id cpPlatform;        // OpenCL platform
	cl_device_id device_id;           // device ID
	cl_context context;               // context
	cl_command_queue queue;           // command queue
	cl_program program;               // program
	cl_kernel kernel = nullptr;			// kernel

	cl_int err;
	// Bind to platform
	err = clGetPlatformIDs(1, &cpPlatform, NULL);

	// Get ID for the device
	err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

	// Create a context  
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);

	// Create a command queue 
	queue = clCreateCommandQueue(context, device_id, 0, &err);

	char* kernelSource = readKernelSource("GameOfLife.cl");

	// Create the compute program from the source buffer
	program = clCreateProgramWithSource(context, 1, (const char**)&kernelSource, NULL, &err);

	// Build the program executable 
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);


	if (err)
	{
		// Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

		// Allocate memory for the log
		char* log = (char*)malloc(log_size);

		// Get the log
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

		// Print the log
		printf("%s\n", log);

		free(log);
	}


	unsigned long deviceGlobalMemSize;
	size_t globalSize[2], localSize[2];
	int kernelNum;

	int userMatrixDimension = -1;


	clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(deviceGlobalMemSize), &deviceGlobalMemSize, NULL);
	unsigned int MaxDimensionSizePerMatrix = sqrt(deviceGlobalMemSize / 3);

	printf("=============================================DOBRODOSLI U CONWAY========================================================\n");

	printf("\nSvijet je predstavljen u obliku kvadratne matrice. Unesite dimenziju svijeta ili -1 za podrazumijevane vrijednosti: \n");
	printf("dimenzija = ");
	scanf(" %d", &userMatrixDimension);


	if (userMatrixDimension <= 0 || userMatrixDimension > MaxDimensionSizePerMatrix) {
		userMatrixDimension = MaxDimensionSizePerMatrix;
	}

	localSize[0] = localSize[1] = LOCAL_SIZE;

	globalSize[0] = (size_t)ceil((size_t)((userMatrixDimension) / (float)localSize[0])) * localSize[0];
	globalSize[1] = (size_t)ceil((size_t)((userMatrixDimension) / (float)localSize[1])) * localSize[1];


	const int worldSize = globalSize[0] * globalSize[1];

	printf("\nUnesite broj pokretanja kernela u kojima ce se izvrsavati obrada slike : \n");
	printf("broj kernela = ");
	scanf(" %d", &kernelNum);

	if (kernelNum < 0 || kernelNum > MAX_KERNEL_NUM) {
		kernelNum = 1;
	}

	unsigned char* matrix = nullptr;

	//inicijalizacija svijeta
	matrix = initMatrix(program, kernel, context, queue, globalSize, localSize, kernelNum);

	//alokacija prostora za matrice koje predstavljaju stanje prije i nakon jedne iteracije
	d_InMatrix = clCreateBuffer(context, CL_MEM_READ_WRITE, worldSize, NULL, NULL);

	d_OutMatrix = clCreateBuffer(context, CL_MEM_READ_WRITE, worldSize, NULL, NULL);

	//Upis podataka na alocirani prostor na disku
	err = clEnqueueWriteBuffer(queue, d_InMatrix, CL_TRUE, 0, worldSize, matrix, 0, NULL, NULL);
	clFinish(queue);

	unsigned int xStart = 1, yStart = 1;
	int imageWidth = -1, imageHeight = -1;
	unsigned char* imageBuffer = nullptr;
	int imageSize;


	char answer;

	//provjera da li korisnik zeli izvrsiti inicijalizaciju svijata slikom, moguce je vise puta unijeti sliku
	kernel = clCreateKernel(program, "initMatrixWithImage", &err);
	do {
		printf("\nDa li zelite inicijalizovati svijet sa slikom?(y/n): ");
		answer = getCharacterWithEscape("yn");

		if (answer == 'y') {
			char repeat;
			do {
				printf("\nUnesite koordinate : \n");
				printf("x = ");
				scanf(" %d", &xStart);
				printf("y = ");
				scanf(" %d", &yStart);

				if (xStart > globalSize[0] || xStart < 0 || yStart > globalSize[1] || yStart < 0) {

					printf("\nIndeksi su izvan granica svijeta(\"sirina\" i \"visina\" svijeta -> %d), da li zelite da pokusate ponovo?(y/n): \n", (int)globalSize[0]);
					repeat = getCharacterWithEscape("yn");
				}
				else {
					std::string path;
					printf("\nUnesite putanju do slike(**Napomena : slika mora biti u .pgm formatu) : \n");
					std::cin >> path;
					readPGMImage(path.c_str(), imageBuffer, imageWidth, imageHeight);

					imageSize = imageWidth * imageHeight;


					//alokacija prostora za sliku koja ce biti ubacena u svijet
					cl_mem d_image;

					d_image = clCreateBuffer(context, CL_MEM_READ_WRITE, static_cast<size_t>(imageWidth) * imageHeight, NULL, NULL);
					err = clEnqueueWriteBuffer(queue, d_image, CL_TRUE, 0, static_cast<size_t>(imageWidth) * imageHeight, imageBuffer, 0, NULL, NULL);

					unsigned int widthBoundary = static_cast<unsigned long long>(xStart) + imageWidth > globalSize[0] ? globalSize[0] : ((size_t)xStart + imageWidth);
					unsigned int heightBoundary = static_cast<unsigned long long>(yStart) + imageHeight > globalSize[1] ? globalSize[1] : ((size_t)yStart + imageHeight);

					//podesavanje parametara kernela
					err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_InMatrix);
					err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_image);
					err |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &xStart);
					err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &yStart);
					err |= clSetKernelArg(kernel, 4, sizeof(unsigned int), &widthBoundary);
					err |= clSetKernelArg(kernel, 5, sizeof(unsigned int), &heightBoundary);
					err |= clSetKernelArg(kernel, 6, sizeof(unsigned int), &globalSize[0]);
					err |= clSetKernelArg(kernel, 7, sizeof(unsigned int), &imageWidth);

					//pokretanje kernela "initMatrixWithImage" koji ima za cilj da inicijalizuje matricu sa unijetom slikom 
					// Execute the kernel over the entire range of the data set  
					err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);

					// Wait for the command queue to get serviced before reading back results
					clFinish(queue);
					//zavrsetak inizijalizacije svijeta sa slikom
					clEnqueueReadBuffer(queue, d_InMatrix, CL_TRUE, 0, worldSize, matrix, 0, NULL, NULL);
					clFinish(queue);

					clReleaseMemObject(d_image);
					break;
				}
			} while (repeat == 'y');
		}
	} while (answer == 'y');


	clReleaseKernel(kernel);
	clFinish(queue);


	int i = 0;
	bool reverse = false;
	int offset = globalSize[0] / kernelNum;

	//pocetak glavnog dijela programa


	//koristim Pixel da reprezentujem RGB sliku koja ce biti sacuvana, dimenzije slike su jednaka dimenzijama svijeta
	Pixel* bufferSeg = nullptr;
	size_t bytes = worldSize * sizeof(Pixel);
	bufferSeg = (Pixel*)malloc(bytes);

	//alokacija prostora na uredjaju, koristim ga pri detekciji oscilatornog obrasca
	cl_mem d_imageSegment = clCreateBuffer(context, CL_MEM_READ_WRITE, bytes, NULL, NULL);

	while (true)
	{
		std::cout << "\n\n-----Opcije :--------\n0 = detekcija + sljedeca iteracija\n1 = sljedeca iteracija\n2 = izdvajanje podsegmenta\n[broj] = iteracija na koju se skace\n-1 = KRAJ IGRE! \n\nunos : ";
		int input;
		scanf("%d", &input);

		if (input == -1) {
			break;
		}
		//korisnik zeli sljedecu iteraciju + detekciju
		else if (input == 0) {
			printf("\nProcesiranje....\n");

			//kernal gameOfLifeOscilator izracunava sljedecu iteraciju, te napraviti RGB sliku od date binarne matrice
			kernel = clCreateKernel(program, "gameOfLifeOscilator", &err);

			//pokrecem kerenel onoliko puta koliko je korisnik specifikovao
			for (size_t kernelIteration = 0, startCol = 0; kernelIteration < kernelNum; kernelIteration++, startCol += offset)
			{

				unsigned int iterationOffset = offset * kernelIteration;

				//podesavanje parametara u kernelu
				err = clSetKernelArg(kernel, 0, sizeof(cl_mem), reverse ? &d_OutMatrix : &d_InMatrix);
				err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), reverse ? &d_InMatrix : &d_OutMatrix);
				err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_imageSegment);
				err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &globalSize[0]);
				err |= clSetKernelArg(kernel, 4, sizeof(unsigned int), &startCol);
				err |= clSetKernelArg(kernel, 5, sizeof(unsigned int), &offset);
				err |= clSetKernelArg(kernel, 6, sizeof(unsigned int), &iterationOffset);

				err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);
				//printf(TranslateOpenCLError(err));


			}
			//cekanje na zavrsetak kernela
			clFinish(queue);
			clReleaseKernel(kernel);

			//kernel detector u prosljedjenoj RGB slici, a na osnovu prethodne i sljedece iteracije, pronalazi oscilatorski obrazac i oznacava ga posebnom bojom
			//slika se cuva u blinkerRGB folderu
			// **napomena : radi se detekcija Blinker-a (uz pomocne funkije isHorizontalBlinker i isVerticalBlinker)
			kernel = clCreateKernel(program, "detector", &err);
			//isti princip kako i gore, razdvojeno je na dva dijela, jer globalna barijera ne radi kako je ocekivano
			for (size_t kernelIteration = 0, startCol = 0; kernelIteration < kernelNum; kernelIteration++, startCol += offset)
			{
				int iterationOffset = offset * kernelIteration;

				err = clSetKernelArg(kernel, 0, sizeof(cl_mem), reverse ? &d_OutMatrix : &d_InMatrix);
				err = clSetKernelArg(kernel, 1, sizeof(cl_mem), reverse ? &d_InMatrix : &d_OutMatrix);
				err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_imageSegment);
				err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &globalSize[0]);
				err |= clSetKernelArg(kernel, 4, sizeof(unsigned int), &startCol);
				err |= clSetKernelArg(kernel, 5, sizeof(unsigned int), &offset);
				err |= clSetKernelArg(kernel, 6, sizeof(unsigned int), &iterationOffset);

				err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);

			}
			clFinish(queue);
			clReleaseKernel(kernel);

			//citanje rgb slike u kojoj su oznaceni svi pronadjeni oscilatorski obrasci
			clEnqueueReadBuffer(queue, d_imageSegment, CL_TRUE, 0, bytes, bufferSeg, 0, NULL, NULL);
			clFinish(queue);

			//cuvanje .ppm slike, detektovani blinkeri su obojeni crvenom bojom
			const std::string outFile2 = std::string("blinkerRGB\\imageWithDetectedBlinkers") + std::to_string(i + 1) + std::string(".ppm");
			writePPMImage(outFile2.c_str(), bufferSeg, globalSize[0], globalSize[1]);

			i++;


			reverse = !reverse;

			printf("\nSegment sa detektovanim obrascima je uspjesno sacuvan!\n");

		}

		//korisnik zeli da izdvoji podsegment
		else if (input == 2) {
			printf("\nProcesiranje...\n");

			int x0, y0, width, height;

			unsigned char* subsegment = nullptr;

			printf("\nUnesi pocetne koordinate: \n");
			printf("x = ");
			scanf(" %d", &x0);
			printf("y = ");
			scanf(" %d", &y0);

			printf("\nUnesi sirinu i visinu podsegmenta: \n");
			printf("sirina = ");
			scanf(" %d", &width);
			printf("visina = ");
			scanf(" %d", &height);

			if (static_cast<unsigned long long>(x0) + width > globalSize[0] || static_cast<unsigned long long>(y0) + height > globalSize[1]) {
				printf("\nPodsegment je van granica svijeta! Cuvanje nije uspjesno!\n");
			}
			else {

				cl_mem d_subsegment;
				d_subsegment = clCreateBuffer(context, CL_MEM_READ_WRITE, (size_t)(width)*height, NULL, NULL);

				subsegment = (unsigned char*)malloc((size_t)width * height);

				//kernel getSubsegment dohvata podsegment iz svijeta i smijesta ga u folder subsegment
				kernel = clCreateKernel(program, "getSubsegment", &err);

				err = clSetKernelArg(kernel, 0, sizeof(cl_mem), reverse ? &d_InMatrix : &d_OutMatrix);
				err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_subsegment);
				err |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &globalSize[0]);
				err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &x0);
				err |= clSetKernelArg(kernel, 4, sizeof(unsigned int), &y0);
				err |= clSetKernelArg(kernel, 5, sizeof(unsigned int), &width);
				err |= clSetKernelArg(kernel, 6, sizeof(unsigned int), &height);

				err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);
				//printf(TranslateOpenCLError(err));

				clFinish(queue);

				//ucitavanje podsegmenta sa memorije gpu-a
				clEnqueueReadBuffer(queue, d_subsegment, CL_TRUE, 0, (size_t)width * height, subsegment, 0, NULL, NULL);
				clFinish(queue);

				//upisivanje podsegemnta
				const std::string outFile = std::string("subsegment\\Subsegment") + std::to_string(i + 1) + std::string(".pgm");
				writePGMImage(outFile.c_str(), convertBinaryToBlackAndWhiteMatrix(subsegment, width * height), width, height);

				printf("\nPodsegment je uspjesno sacuvan!\n");
				i++;
				clReleaseKernel(kernel);
				clReleaseMemObject(d_subsegment);
			}
		}
		//korisnik je unio 1, odnosno zeli narednu iteraciju ili je unio [broj] odnosno zeli iteraciju koja je jednaka trenutnaIteracija + [broj]
		else {

			printf("\nProcesiranje...\n");
			//kernel gameOfLife izracunava sljedecu(ili specifikovanu) iteraciju, te rezultat cuva u folderu regularIteration
			kernel = clCreateKernel(program, "gameOfLife", &err);
			//i -> trenutna iteracija    input -> [broj]   finish -> krajnja iteracija
			int finish = i + input;

			for (; i < finish; i++)
			{
				//pokrecem onoliko kernela koliko je korisnik specifikovao
				for (size_t kernelIteration = 0, startCol = 0; kernelIteration < kernelNum; kernelIteration++, startCol += offset)
				{
					int iterationOffset = offset * kernelIteration;
					err = clSetKernelArg(kernel, 0, sizeof(cl_mem), reverse ? &d_OutMatrix : &d_InMatrix);
					err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), reverse ? &d_InMatrix : &d_OutMatrix);
					err |= clSetKernelArg(kernel, 2, sizeof(int), &globalSize[0]);
					err |= clSetKernelArg(kernel, 3, sizeof(int), &startCol);
					err |= clSetKernelArg(kernel, 4, sizeof(int), &offset);
					err |= clSetKernelArg(kernel, 5, sizeof(int), &iterationOffset);
					err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);
				}

				clFinish(queue);

				reverse = !reverse;
			}

			//ucitavanje slike radi cuvanja
			clEnqueueReadBuffer(queue, reverse ? d_InMatrix : d_OutMatrix, CL_TRUE, 0, worldSize, matrix, 0, NULL, NULL);
			clFinish(queue);

			//cuvanje slike
			const std::string outFile = std::string("regularIteration\\image") + std::to_string(i + 1) + std::string(".pgm");
			writePGMImage(outFile.c_str(), convertBinaryToBlackAndWhiteMatrix(matrix, globalSize[0] * globalSize[1]), globalSize[0], globalSize[1]);

			// Wait for the command queue to get serviced before reading back results
			clFinish(queue);

			printf("\nIteracije su odradjene. Slika je uspjesno sacuvana!\n");
			clReleaseKernel(kernel);
		}

	}

	// release OpenCL resources
	clReleaseMemObject(d_InMatrix);
	clReleaseMemObject(d_OutMatrix);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	free(kernelSource);
}

unsigned char* initMatrix(cl_program program, cl_kernel kernel, cl_context context, cl_command_queue queue, size_t* globalSize, size_t* localSize, int kernelNum) {


	cl_mem d_matrix;

	unsigned char* matrix = nullptr;

	cl_int err;

	matrix = (unsigned char*)malloc(globalSize[0] * globalSize[1]);

	// Create the compute kernel in the program we wish to run
	kernel = clCreateKernel(program, "initMatrix", &err);

	// alokacija prostara za inicijalnu matricu na kernelu
	d_matrix = clCreateBuffer(context, CL_MEM_READ_WRITE, globalSize[0] * globalSize[1], NULL, NULL);

	int offset = globalSize[0] / kernelNum;

	//pokrecem onoliko kernela koliko je korisnik specifikovao
	for (size_t kernelIteration = 0, startCol = 0; kernelIteration < kernelNum; kernelIteration++, startCol += offset)
	{

		unsigned int iterationOffset = offset * kernelIteration;
		// Set the arguments to our compute kernel
		err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_matrix);
		err |= clSetKernelArg(kernel, 1, sizeof(unsigned int), &globalSize[0]);
		err |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &globalSize[1]);
		err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &offset);
		err |= clSetKernelArg(kernel, 4, sizeof(unsigned int), &iterationOffset);
		err |= clSetKernelArg(kernel, 5, sizeof(unsigned int), &startCol);

		// Execute the kernel over the entire range of the data set  
		err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);
	}

	clFinish(queue);


	char answer = -1;
	int xStart, yStart;
	kernel = clCreateKernel(program, "userInitMatrix", &err);
	do {
		printf("\nDa li zelite inicijalizovati svijet rucno?(y/n): ");
		answer = getCharacterWithEscape("yn");

		if (answer == 'y') {
			char repeat = -1;
			do {
				printf("\nUnesite koordinate : \n");
				printf("x = ");
				scanf(" %d", &xStart);
				printf("y = ");
				scanf(" %d", &yStart);

				if (xStart > globalSize[0] || xStart < 0 || yStart > globalSize[1] || yStart < 0) {

					printf("\nIndeksi su izvan granica svijeta(\"sirina\" i \"visina\" svijeta -> %d), da li zelite da pokusate ponovo?(y/n): \n", (int)globalSize[0]);
					repeat = getCharacterWithEscape("yn");
				}
				else {
					for (size_t kernelIteration = 0, startCol = 0; kernelIteration < kernelNum; kernelIteration++, startCol += offset)
					{

						unsigned int iterationOffset = offset * kernelIteration;
						//podesavanje parametara kernela
						err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_matrix);

						err |= clSetKernelArg(kernel, 1, sizeof(unsigned int), &iterationOffset);
						err |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &xStart);
						err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &yStart);
						err |= clSetKernelArg(kernel, 4, sizeof(unsigned int), &globalSize[0]);

						// Execute the kernel over the entire range of the data set  
						err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);

					}
					// Wait for the command queue to get serviced before reading back results
					clFinish(queue);

				}
			} while (repeat == 'y');
		}
	} while (answer == 'y');


	clEnqueueReadBuffer(queue, d_matrix, CL_TRUE, 0, globalSize[0] * globalSize[1], matrix, 0, NULL, NULL);

	clFinish(queue);

	clReleaseKernel(kernel);

	return matrix;

}