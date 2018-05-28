/*
check_zeros
Description: App to check the quality of the biodiversity rasters (Projeto Global)
Version: 0.1
Eduardo Ribeiro Lacerda <eduardolacerdageo@gmail.com>
International Institute for Sustainability (IIS) - 2018
*/

// STL
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

// GDAL
#include <gdal_priv.h>

// BOOST
#include <boost\filesystem.hpp>

namespace fs = boost::filesystem;

void get_all(const fs::path& root, const std::string& ext, std::vector<fs::path>& ret) {

	if (!fs::exists(root) || !fs::is_directory(root)) return;

	fs::directory_iterator it(root);
	fs::directory_iterator endit;

	while (it != endit)
	{
		if (fs::is_regular_file(*it) && it->path().extension() == ext) ret.push_back(it->path());
		++it;
	}

}

void check_zeros(const fs::path& filepath) {

	GDALAllRegister();
	GDALDataset *ds;
	bool zero = true;
	int nRows;
	int nCols;
	std::string file = filepath.string();

	ds = (GDALDataset*)GDALOpen(file.c_str(), GA_ReadOnly);

	nRows = ds->GetRasterBand(1)->GetYSize();
	nCols = ds->GetRasterBand(1)->GetXSize();

	uint8_t *ds_row = (uint8_t *)CPLMalloc(sizeof(uint8_t)*nCols);

	for (int i = 0; i < nRows; i++) {
		ds->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, ds_row, nCols, 1, GDT_Byte, 0, 0);

		for (int j = 0; j < nCols; j++) {
			if (ds_row[j] != 0) {
				zero = false;
				break;
			}
		}
	}

	if (zero) {
		std::cout << "Imagem com problema: " << filepath.filename() << std::endl;
	}
	else {
		std::cout << "Imagem ok: " << filepath.filename() << std::endl;
	}

	CPLFree(ds_row);
	GDALClose(ds);
}


int main(int argc, char** argv) {

	fs::path p(argv[1]);
	std::string extension = argv[2];
	std::vector<fs::path> filelist;
	get_all(p, extension, filelist);

	srand(time(NULL));
	for (int i = 0; i < filelist.size(); i++) {
		int randomIndex = rand() % filelist.size();
		check_zeros(filelist.at(randomIndex));
	}

	// for (auto s : filelist) {
	// 	check_zeros(s);
	// }

	std::cin.get();
	return 0;
}