#define _USE_MATH_DEFINES
#define number 284
#define Wid 1920
#define Hgt 1080
#define tnum 1

#include <stdio.h>
#include <cmath>
#include <vector>
#include <sys/time.h>

#pragma pack(push,1)
typedef struct tagBITMAPFILEHEADER
{
	unsigned short bfType;
	int            bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	int            bf0ffBits;
}BITMAPFILEHEADER;

#pragma pack(pop)

typedef struct tagBITMAPINFOHEADER
{
	int             biSize;
	int			    biWidth;
	int			    biHeight;
	unsigned short  biPlanes;
	unsigned short  biBitCount;
	int             biCompression;
	int             biSizeImage;
	int			    biXPelsPerMeter;
	int			    biYPelsPerMeter;
	int             biCirUsed;
	int             biCirImportant;
}BITMAPINFOHEADER;

typedef struct tagRGBQUAD
{
	unsigned char  rgbBlue;
	unsigned char  rgbGreen;
	unsigned char  rgbRed;
	unsigned char  rgbReserved;
}RGBQUAD;

typedef struct tagBITMAPINFO
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD          bmiColors[1];
}BITMAPINFO;

/*時間計測*/
double gettimeofday_sec()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec * 1e-6;
}

unsigned char img[Hgt*Wid];
double I[Hgt*Wid];

int main()
{
	BITMAPFILEHEADER    BmpFileHeader;
	BITMAPINFOHEADER    BmpInfoHeader;
	RGBQUAD             RGBQuad[256];

	BmpFileHeader.bfType = 19778;
	BmpFileHeader.bfSize = 14 + 40 + 1024 + (256 * 256);
	BmpFileHeader.bfReserved1 = 0;
	BmpFileHeader.bfReserved2 = 0;
	BmpFileHeader.bf0ffBits = 14 + 40 + 1024;

	BmpInfoHeader.biSize = 40;
	BmpInfoHeader.biWidth = 1920;
	BmpInfoHeader.biHeight = 1080;
	BmpInfoHeader.biPlanes = 1;
	BmpInfoHeader.biBitCount = 8;
	BmpInfoHeader.biCompression = 0L;
	BmpInfoHeader.biSizeImage = 0L;
	BmpInfoHeader.biXPelsPerMeter = 0L;
	BmpInfoHeader.biYPelsPerMeter = 0L;
	BmpInfoHeader.biCirUsed = 0L;
	BmpInfoHeader.biCirImportant = 0L;

	int i, j, n;

	for (i = 0; i<256; i++){
		RGBQuad[i].rgbBlue = i;
		RGBQuad[i].rgbGreen = i;
		RGBQuad[i].rgbRed = i;
		RGBQuad[i].rgbReserved = 0;
	}

	FILE *fp;

	fp = fopen("cube284.3d", "rb");
	fread(&n, sizeof(int), 1, fp);

	int x[number], y[number], x1, y1, z1;
	double z[number];

	for (i = 0; i<number; i++){
		fread(&x1, sizeof(int), 1, fp);
		fread(&y1, sizeof(int), 1, fp);
		fread(&z1, sizeof(int), 1, fp);

		x[i] = x1 * 40 + Hgt / 2;
		y[i] = y1 * 40 + Wid / 2;
		z[i] = (double)z1 * 40 + 5000.0;
	}

	fclose(fp);

	double  tmp = 0.0;
	int s, q;


	double starttime, endtime, time_tmp, adtime_tmp, avgtime_tmp;

	for(q = 0; q < tnum; q++){

		for (i = 0; i < Hgt; i++){
			for (j = 0; j < Wid; j++){
				I[i * Wid +j] = 0.0;
			}
		}

		starttime=gettimeofday_sec();

		for (i = 0; i < Hgt; i++){
			for (j = 0; j < Wid; j++){

				for (s = 0; s<number; s++){

					tmp = tmp + cos(4.9630216 * (((i - x[s])*(i - x[s])) + ((j - y[s])*(j - y[s]))) / z[s]);
				}

				I[i * Wid +j] = tmp;
				tmp = 0.0;
			}
		}

		endtime=gettimeofday_sec();

		time_tmp = endtime-starttime;
		adtime_tmp += time_tmp;
		printf("%lf\n",time_tmp);

		time_tmp = 0.0;

	}

	avgtime_tmp = adtime_tmp / tnum;

	printf("AddCalculation time is %lf[sec]\n",adtime_tmp);
	printf("AverageCalculation time is %lf[sec]\n",avgtime_tmp);


	double max_tmp = 0.0, min_tmp = 0.0, mid_tmp = 0.0;

	max_tmp = I[0];
	min_tmp = I[0];

	for (i = 0; i < Hgt*Wid; i++){
			if (max_tmp <= I[i]){
				max_tmp = I[i];
			}

			else if (min_tmp > I[i]){
				min_tmp = I[i];
		}
	}

	mid_tmp = (max_tmp + min_tmp) / 2;

	printf("max = %lf\n", max_tmp);
	printf("min = %lf\n", min_tmp);
	printf("mid = %lf\n", mid_tmp);

	for (i = 0; i < Hgt*Wid; i++){
			img[i] = 0;
	}

	for (i = 0; i < Hgt*Wid; i++){

			if (I[i] <= mid_tmp){
				img[i] = 0;
			}

			else if (I[i] > mid_tmp){
				img[i] = 255;
			}
	}

	fp = fopen("CGH.bmp", "wb");

	fwrite(&BmpFileHeader, sizeof(BmpFileHeader), 1, fp);
	fwrite(&BmpInfoHeader, sizeof(BmpInfoHeader), 1, fp);
	fwrite(&RGBQuad[0], sizeof(RGBQuad[0]), 256, fp);
	fwrite(img, sizeof(unsigned char), Hgt * Wid, fp);

	fclose(fp);

	return 0;
}
