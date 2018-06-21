#include <iostream>
#include <string>
#include <vector>
#include "gdal_priv.h"

using namespace std;

void process_image(const string& in_image, const string& out_image) {
	GDALDataset  *inputDs;
    GDALDataset  *outputDs;
    GDALAllRegister();

    GDALDriver *geoTiff;
    int nRows, nCols;
    double noData;
    double transform[6];

    inputDs = (GDALDataset *) GDALOpen(in_image.c_str(), GA_ReadOnly);
   	nRows = inputDs->GetRasterBand(1)->GetYSize();
    nCols = inputDs->GetRasterBand(1)->GetXSize();
    noData = inputDs->GetRasterBand(1)->GetNoDataValue();
    inputDs->GetGeoTransform(transform);

    // Create output source imagem
    geoTiff = GetGDALDriverManager()->GetDriverByName("GTiff");
    outputDs = geoTiff->Create(out_image.c_str(), nCols, nRows, 1, GDT_Int32, NULL);
    outputDs->SetGeoTransform(transform);
    // outputDs->GetRasterBand(1)->SetNoDataValue(noData);

    int *input_Row = (int*)CPLMalloc(sizeof(int)*nCols);
    int *output_Row = (int*)CPLMalloc(sizeof(int)*nCols);

	for (int i = 0; i < nRows; i++)
	{
	    inputDs->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, input_Row, nCols, 1, GDT_Int32, 0, 0);
	    for (int j = 0; j < nCols; j++)
	    {
	        if (input_Row[j] == noData)
	        {
	            output_Row[j] = noData;
	        }
	        else if (input_Row[j] == 0 && input_Row[j-1] != noData && input_Row[j-1] != 0) 
	        {
				output_Row[j] = input_Row[j-1];
	        }
	        else if (input_Row[j] == 0 && input_Row[j+1] != noData && input_Row[j+1] != 0) 
	        {
				output_Row[j] = input_Row[j+1];
	        }
	        else
	        {
	            output_Row[j] = input_Row[j];
	        }
	    }
	    outputDs->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, output_Row, nCols, 1, GDT_Int32, 0, 0);
	}

    // Delete buffers
    CPLFree(input_Row);
    CPLFree(output_Row);

    // Delete datasets
    GDALClose(inputDs);
    GDALClose(outputDs);
    GDALDestroyDriverManager();

   	puts("concluído.");
}

int main(int argc, char** argv) {
	// configure somente estas duas linhas
	string original = "/home/elacerda/Documents/delta_layers/Carbon_95_Combine.tif";
	int num_iterations = 25;

	// loop principal
	for (int i = 0; i < num_iterations; i++) {
		string iter = "/home/elacerda/Documents/delta_layers/iter";
		iter.append(to_string(i));
		iter.append(".tif");
		cout << "Processando iteração: " << i << endl;
		process_image(original, iter);
	}

	return 0;
}	


// g++ -Wall -o teste teste.cpp -I /usr/include/gdal -lgdal