#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <clocale>
#include "gdal_priv.h"

using namespace std;

void mult(const string& dType,
		  const string& input_1_path,
		  const string& input_2_path,
		  const string& output_path);

int main(int argc, char** argv) {

	setlocale(LC_ALL, "Portuguese");


	if (argc != 5) {
		cout << "#################################################################################" << endl;
		cout << "Erro!" << endl;
		cout << "Utilização:" << endl;
		cout << "mult <tipo do dado> <imagem de entrada 1> <imagem de entrada 2> <imagem de saída>" << endl;
		cout << "Exemplo:" << endl;
		cout << "mult -byte input1.tif input2.tif output.tif" << endl;
		cout << "#################################################################################" << endl;
		exit(1);
	}

	string dType = argv[1];
	string input_1_filename = argv[2];
	string input_2_filename = argv[3];
	string output_filename = argv[4];

/*	vector< string > args;

	if (argc != 5) {
		cerr << "Estão faltando argumentos!" << endl;
	} else {
	
		for (size_t i = 0; i < argc; i++) {
			if (i + 1 < argc)
				args.push_back(argv[i+1]);
		}

		for (size_t i = 1; i < args.size(); i++) {
			if (args[i] == "-byte") {		
				dType = args[i];
				i++; continue;
			} else if (args[i] == "-int") {
				dType = args[i];
				i++; continue;
			} else if (args[i] == "-uint") {
				dType = args[i];
				i++; continue;
			} else if (args[i] == "-float") {
				dType = args[i];
				i++; continue;
			} else {
				cout << "Tipo de dado da imagem inválida ou não especificada." << endl;
				cout << "Opções válidas: -byte, -int, -uint ou -float" << endl;
			}
		}
	}*/

	puts("Iniciando processamento...");
	mult(dType, input_1_filename, input_2_filename, output_filename);

	puts("Processamento concluído.");
	cin.get();
	return 0;
}

void mult(const string& dType,
		  const string& input_1_path,
		  const string& input_2_path,
		  const string& output_path) 
{	

	GDALDataset* input_1;	
	GDALDataset* input_2;	
	GDALDataset* output;

	GDALAllRegister();
	GDALDriver *geoTiff;

	int nRows, nCols;
	double noData;
	double transform[6];
	const char* proj;

	input_1 = (GDALDataset*)GDALOpen(input_1_path.c_str(), GA_ReadOnly);
	input_2 = (GDALDataset*)GDALOpen(input_2_path.c_str(), GA_ReadOnly);

	nRows = input_1->GetRasterBand(1)->GetYSize();
	nCols = input_1->GetRasterBand(1)->GetXSize();
	noData = input_1->GetRasterBand(1)->GetNoDataValue();
	input_1->GetGeoTransform(transform);
	proj = input_1->GetProjectionRef();	

	cout << endl;
	cout << "Info imagem: " << endl;

	for (int i = 0; i < 6; i++)
	{
        cout << transform[i] << endl;
	}

	cout << endl;
	cout << "Proj: "<< endl << proj << endl;
	cout << endl;

	if (dType == "-byte") {
		geoTiff = GetGDALDriverManager()->GetDriverByName("GTiff");
		output = geoTiff->Create(output_path.c_str(), nCols, nRows, 1, GDT_Byte, NULL);
		output->SetGeoTransform(transform);
		output->GetRasterBand(1)->SetNoDataValue(noData);
		output->SetProjection(proj);

		uint8_t *input_1_Row = (uint8_t*)CPLMalloc(sizeof(uint8_t)*nCols);
		uint8_t *input_2_Row = (uint8_t*)CPLMalloc(sizeof(uint8_t)*nCols);
		uint8_t *output_Row = (uint8_t*)CPLMalloc(sizeof(uint8_t)*nCols);

		for (int i = 0; i < nRows; i++)
		{
			input_1->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, input_1_Row, nCols, 1, GDT_Byte, 0, 0);
			input_1->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, input_2_Row, nCols, 1, GDT_Byte, 0, 0);
			for (int j = 0; j < nCols; j++)
			{
				if (input_1_Row[j] || input_2_Row[j] == noData)
				{
					output_Row[j] = 0;
				} 
				else
				{
					output_Row[j] = input_1_Row[j] * input_2_Row[j];
				}
			}
			output->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, output_Row, nCols, 1, GDT_Byte, 0, 0);
		}

		CPLFree(input_1_Row);
		CPLFree(input_2_Row);
		CPLFree(output_Row);

		GDALClose(input_1);
		GDALClose(input_2);
		GDALClose(output);
		GDALDestroyDriverManager();
	} else if (dType == "-int") {
		geoTiff = GetGDALDriverManager()->GetDriverByName("GTiff");
		output = geoTiff->Create(output_path.c_str(), nCols, nRows, 1, GDT_Int32, NULL);
		output->SetGeoTransform(transform);
		output->GetRasterBand(1)->SetNoDataValue(noData);
		output->SetProjection(proj);

		int32_t *input_1_Row = (int32_t*)CPLMalloc(sizeof(int32_t)*nCols);
		int32_t *input_2_Row = (int32_t*)CPLMalloc(sizeof(int32_t)*nCols);
		int32_t *output_Row = (int32_t*)CPLMalloc(sizeof(int32_t)*nCols);

		for (int i = 0; i < nRows; i++)
		{
			input_1->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, input_1_Row, nCols, 1, GDT_Int32, 0, 0);
			input_2->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, input_2_Row, nCols, 1, GDT_Int32, 0, 0);
			for (int j = 0; j < nCols; j++)
			{
				if (input_1_Row[j] || input_2_Row[j] == noData)
				{
					output_Row[j] = 0;
				} 
				else
				{
					output_Row[j] = input_1_Row[j] * input_2_Row[j];
				}
			}
			output->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, output_Row, nCols, 1, GDT_Int32, 0, 0);
		}

		CPLFree(input_1_Row);
		CPLFree(input_2_Row);
		CPLFree(output_Row);

		GDALClose(input_1);
		GDALClose(input_2);
		GDALClose(output);
		GDALDestroyDriverManager();
	} else if (dType == "-uint") {
		geoTiff = GetGDALDriverManager()->GetDriverByName("GTiff");
		output = geoTiff->Create(output_path.c_str(), nCols, nRows, 1, GDT_UInt32, NULL);
		output->SetGeoTransform(transform);
		output->GetRasterBand(1)->SetNoDataValue(noData);
		output->SetProjection(proj);

		uint32_t *input_1_Row = (uint32_t*)CPLMalloc(sizeof(uint32_t)*nCols);
		uint32_t *input_2_Row = (uint32_t*)CPLMalloc(sizeof(uint32_t)*nCols);
		uint32_t *output_Row = (uint32_t*)CPLMalloc(sizeof(uint32_t)*nCols);

		for (int i = 0; i < nRows; i++)
		{
			input_1->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, input_1_Row, nCols, 1, GDT_UInt32, 0, 0);
			input_2->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, input_2_Row, nCols, 1, GDT_UInt32, 0, 0);
			for (int j = 0; j < nCols; j++)
			{
				if (input_1_Row[j] || input_2_Row[j] == noData)
				{
					output_Row[j] = 0;
				} 
				else
				{
					output_Row[j] = input_1_Row[j] * input_2_Row[j];
				}
			}
			output->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, output_Row, nCols, 1, GDT_UInt32 , 0, 0);
		}

		CPLFree(input_1_Row);
		CPLFree(input_2_Row);
		CPLFree(output_Row);

		GDALClose(input_1);
		GDALClose(input_2);
		GDALClose(output);
		GDALDestroyDriverManager();
	} else if (dType == "-float") {
		geoTiff = GetGDALDriverManager()->GetDriverByName("GTiff");
		output = geoTiff->Create(output_path.c_str(), nCols, nRows, 1, GDT_Float32, NULL);
		output->SetGeoTransform(transform);
		output->GetRasterBand(1)->SetNoDataValue(noData);
		output->SetProjection(proj);

		float *input_1_Row = (float*)CPLMalloc(sizeof(float)*nCols);
		float *input_2_Row = (float*)CPLMalloc(sizeof(float)*nCols);
		float *output_Row = (float*)CPLMalloc(sizeof(float)*nCols);

		for (int i = 0; i < nRows; i++)
		{
			input_1->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, input_1_Row, nCols, 1, GDT_Float32, 0, 0);
			input_2->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, input_2_Row, nCols, 1, GDT_Float32, 0, 0);
			for (int j = 0; j < nCols; j++)
			{
				if (input_1_Row[j] || input_2_Row[j] == noData)
				{
					output_Row[j] = 0;
				} 
				else
				{
					output_Row[j] = input_1_Row[j] * input_2_Row[j];
				}
			}
			output->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, output_Row, nCols, 1, GDT_Float32 , 0, 0);
		}

		CPLFree(input_1_Row);
		CPLFree(input_2_Row);
		CPLFree(output_Row);

		GDALClose(input_1);
		GDALClose(input_2);
		GDALClose(output);
		GDALDestroyDriverManager();
	} else {
		cout << "Tipo de dado errado. Possibilidades: byte, int, uint e float" << endl;
	}
}

// g++ -o mult mult.cpp -I /usr/include/gdal -lgdal