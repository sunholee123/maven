#ifndef IMAGE_H
#define IMAGE_H

#include <QString>
#include <QImage>

#include <dcmtk/dcmdata/dctk.h>
#include "NiftiImage.h"

#define CORONAL 0
#define SAGITTAL 1
#define AXIAL 2

typedef vector<vector<vector<float>>> vec3df; // vector - 3d - float

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

    void setIntensity(double value);
    double getIntensity();

    QString getFileName();
	float getImgVal(size_t i, size_t j, size_t k);
	void setImgVal(size_t i, size_t j, size_t k, float value);

    size_t dx();
    size_t dy();
    size_t dz();
    float sx();
    float sy();
    float sz();

    void setDCMInfo(DicomInfo dcminfo);
    DicomInfo getDCMInfo();
	//bool saveImageFile(QString filename, vec3df *slabvol);
	void saveImageFile(QString filename, vec3df array3D, QString reffilename);
	inline bool isFileExists();
	void setFileName(QString filename);
	void open();

private:
    NiftiImage *img = NULL;
    QString filename;
//    vec3df imgvol;
	float *imgvol;
    QImage T1Images[3];

    double intensity;
    void setDefaultIntensity();
    void setSliceNum();
    size_t dimX, dimY, dimZ;  // size of dimensions
    float sizeX, sizeY, sizeZ;  // size of voxels

    DicomInfo dcminfo;

	float* arr3Dto1D(vec3df array3D);

};


#endif // IMAGE_H
