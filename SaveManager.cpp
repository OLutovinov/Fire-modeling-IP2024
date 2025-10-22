#include "IndProj.h"
#include <fstream>

void SaveManager::SaveData(Cell** field, int width, int height, TCHAR* path)
{
	ofstream output(path);

	output << width << ' ' << height << '\n';
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			Cell current = field[x][y];
			output << current.burning;
			output << ' ' << current.fuelMass;
			output << ' ' << current.burnSpeed;
			output << ' ' << current.capacity;
			output << ' ' << current.combustionHeat;
			output << ' ' << current.temp;
			output << ' ' << current.fireTemp;
			output << ' ' << current.height;
			output << ' ' << current.waterMass;
			output << ' ' << current.waterTemp;
			output << ' ' << current.isWater;
			output << ' ' << current.colorR << ' ' << current.colorG << ' ' << current.colorB;
			output << '\n';
		}
	}
	output.close();
}

ImportedData SaveManager::LoadData(TCHAR* path)
{
	ifstream input(path);

	int width, height;
	input >> width >> height;
	ImportedData result = ImportedData(width, height);

	result.field = new Cell * [width];
	for (int x = 0; x < width; x++)
	{
		result.field[x] = new Cell[height]();
	}

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			input >> result.field[x][y].burning;
			input >> result.field[x][y].fuelMass;
			input >> result.field[x][y].burnSpeed;
			input >> result.field[x][y].capacity;
			input >> result.field[x][y].combustionHeat;
			input >> result.field[x][y].temp;
			input >> result.field[x][y].fireTemp;
			input >> result.field[x][y].height;
			input >> result.field[x][y].waterMass;
			input >> result.field[x][y].waterTemp;
			input >> result.field[x][y].isWater;
			input >> result.field[x][y].colorR >> result.field[x][y].colorG >> result.field[x][y].colorB;
		}
	}
	input.close();
	return result;
}