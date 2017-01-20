#ifndef LCMODELDATA_H
#define LCMODELDATA_H

#include <string>
#include <map>
#include <fstream>
#include <QString>
#include <QStringList>
#include <QDirIterator>

using namespace std;

struct Metabolite
{
	string name;
	float conc;
	int sd;
	float ratio;
	bool qc;
};

struct TableInfo
{
	map<string, Metabolite> metaInfo;
	float fwhm;
	int snr;
	float pvc;
	bool isAvailable = false;
};

class LCModelData
{
public:
	LCModelData();
	TableInfo ***tables = NULL;
	QStringList metaList;
	TableInfo parseTable(string filename);

	bool setLCMInfo(QString dir, const int mrsiVoxNumX, const int mrsiVoxNumY, const int mrsiVoxNumZ);
	QStringList getMetaList();
};

#endif // LCMODELDATA_H
