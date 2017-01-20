#include "LCModelData.h"

LCModelData::LCModelData()
{

}

bool LCModelData::setLCMInfo(QString dir, const int mrsiVoxNumX, const int mrsiVoxNumY, const int mrsiVoxNumZ)
{
	QDirIterator it(dir, QStringList() << "*.table", QDir::Files, QDirIterator::Subdirectories);

	if (!it.hasNext())
	{
//		QMessageBox::critical(this, "Error!", "Can't find *.table files.", QMessageBox::Ok);
		return false;
	}
	if (tables != NULL)
	{
		delete tables;
	}
	tables = new TableInfo **[mrsiVoxNumZ];

	for (int i = 0; i < mrsiVoxNumZ; i++)
	{
		tables[i] = new TableInfo*[mrsiVoxNumY];

		for (int j = 0; j < mrsiVoxNumY; j++)
			tables[i][j] = new TableInfo[mrsiVoxNumX];
	}

	int filecount = 0;
	while (it.hasNext())
	{
		it.next();
		string filename = it.fileName().toStdString();
		string filepath = it.filePath().toStdString();	// path + name
		size_t index1 = filename.find("_");
		size_t index2 = filename.find("-");
		size_t index3 = filename.find(".");
		int x = filename.at(index1 - 1) - '0';
		int y = stoi(filename.substr(index1 + 1, index2 - 1));
		int z = stoi(filename.substr(index2 + 1, index3 - 1));
		tables[x - 1][y - 1][z - 1] = parseTable(filepath);
		filecount++;
	}
}

TableInfo LCModelData::parseTable(string filename)
{
	TableInfo table;
	table.isAvailable = false;
	char line[255];
	ifstream myfile(filename);

	if (myfile.is_open())
	{
		char *token = NULL;
		char s[] = " \t";

		while (myfile.getline(line, 255))
		{
			if (strstr(line, "Conc."))
			{
				while (myfile.getline(line, 255))
				{
					Metabolite metainfo;
					token = strtok(line, s);

					if (token == NULL)	// the last of metabolite parts
						break;

					metainfo.conc = stof(token);
					token = strtok(NULL, s);
					metainfo.sd = stoi(token);
					token = strtok(NULL, s);
					// exception check (1.7E+03+MM17, 0.000-MM17,...)
					QString tempstr = token;
					QStringList t;
					t = tempstr.split(QRegExp("[0-9][+-]"));

					if (t.length() == 2)
					{
						//metainfo.ratio = stof(t[0].toStdString());
						metainfo.ratio = stof(tempstr.left(t[0].length() + 1).toStdString());
						metainfo.name = t[1].toStdString();
					}
					else
					{
						metainfo.ratio = stof(token);
						token = strtok(NULL, s);
						metainfo.name = token;
					}

					metainfo.qc = true;
					table.metaInfo[metainfo.name] = metainfo;

					if (metaList.empty() || !metaList.contains(QString::fromStdString(metainfo.name)))   // To-do: call routine just once
						metaList.push_back(QString::fromStdString(metainfo.name));
				}
			}
			else if (strstr(line, "FWHM"))
			{
				token = strtok(line, "FWHM = ");
				table.fwhm = stof(token);
				token = strtok(NULL, " ppm    S/N =   ");
				table.snr = stoi(token);
				table.isAvailable = true;
			}
		}

		myfile.close();
	}

	return table;
}

