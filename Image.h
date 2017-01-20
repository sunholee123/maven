#ifndef IMAGE_H
#define IMAGE_H

#include <QString>
#include <QImage>
#include <QFileInfo>

#include <dcmtk/dcmdata/dctk.h>
#include "NiftiImage.h"

#define CORONAL 0
#define SAGITTAL 1
#define AXIAL 2

struct DicomInfo {
    Float32 coordFH, coordAP, coordRL;
    Float32 angleFH, angleAP, angleRL;

    friend DicomInfo operator-(const DicomInfo& a, const DicomInfo& b)
	{
        DicomInfo result;
        result.coordFH = a.coordFH - b.coordFH;
        result.coordAP = a.coordAP - b.coordAP;
        result.coordRL = a.coordRL - b.coordRL;
        result.angleFH = a.angleFH - b.angleFH;
        result.angleAP = a.angleAP - b.angleAP;
        result.angleRL = a.angleRL - b.angleRL;
        return result;
    }
};

class Image
{
public:
	Image();
    Image(QString filename, char rw);
    ~Image();

	void open(QString filename, char rw);

	size_t dx();
    size_t dy();
    size_t dz();
    float sx();
    float sy();
    float sz();

	double getIntensity();
	float getImgVal(size_t i, size_t j, size_t k);
	QString getFileName();
	QString getFileName(QString appstr);
	QString getFileBaseName();
	QString getFileExtName();
	QString getFilePath();
	QImage getPlaneImage(int planetype, int slicenum);
	DicomInfo getDCMInfo();


	void setIntensity(double value);
    void setDCMInfo(DicomInfo dcminfo);
	void setFileName(QString filename);
	void setImgVal(size_t i, size_t j, size_t k, float value);
	void setBlankImgvol(size_t size_x, size_t size_y, size_t size_z);

	inline bool isFileExists();
	bool isAvailable();
	void setOverlay(bool isoverlay);
	bool isOverlay();

	void saveImageFile(QString filename, Image* refimg);

private:
    NiftiImage *img = NULL;
	float *imgvol = NULL;
    QString filename;

    double intensity;
    void setDefaultIntensity();
    size_t dimX, dimY, dimZ;  // size of dimensions
    float sizeX, sizeY, sizeZ;  // size of voxels

    DicomInfo dcminfo;

	bool available = false;
	bool overlay = false;
};


#endif // IMAGE_H
