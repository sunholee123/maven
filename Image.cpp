#include "Image.h"

Image::Image()
{
	img = new NiftiImage();
}

Image::Image(QString filename, char rw)
{
	img = new NiftiImage();
	this->filename = filename;
	if (isFileExists())
	{
		img->open(filename.toStdString(), rw);
		dimX = img->nx();
		dimY = img->ny();
		dimZ = img->nz();
		sizeX = img->dx();
		sizeY = img->dy();
		sizeZ = img->dz();
		imgvol = img->readAllVolumes<float>();
		img->close();
		setDefaultIntensity();
	}
}

Image::~Image()
{
//    img->close();
	delete img;
}

void Image::setIntensity(double value)
{
	intensity = value;
}
double Image::getIntensity()
{
	return intensity;
}
QString Image::getFileName()
{
	return filename;
}

float Image::getImgVal(size_t i, size_t j, size_t k)
{
	return imgvol[i + j * dimX + k * dimX * dimY];
}


void Image::setImgVal(size_t i, size_t j, size_t k, float value)
{
	imgvol[i + j * dimX + k * dimX * dimY] = value;
}

void Image::setDefaultIntensity()
{
	float maxval = 0;

	for (size_t i = 0; i < dimX; i++)
	{
		for (size_t j = 0; j < dimY; j++)
		{
			for (size_t k = 0; k < dimZ; k++)
			{
				if (getImgVal(i, j, k) > maxval)
					maxval = getImgVal(i, j, k);
			}
		}
	}

	intensity = 300 / maxval;
}

size_t Image::dx()
{
	return dimX;
}
size_t Image::dy()
{
	return dimY;
}
size_t Image::dz()
{
	return dimZ;
}
float Image::sx()
{
	return sizeX;
}
float Image::sy()
{
	return sizeY;
}
float Image::sz()
{
	return sizeZ;
}

void Image::setDCMInfo(DicomInfo dicominfo)
{
	dcminfo = dicominfo;
}

DicomInfo Image::getDCMInfo()
{
	return dcminfo;
}

void Image::saveImageFile(QString filename, vec3df array3D, QString reffilename)
{
	NiftiImage temp;
	temp.open(reffilename.toStdString(), 'r');
	float *array1D = temp.readAllVolumes<float>();

	size_t dimX = (array3D).size();
	size_t dimY = (array3D)[0].size();
	size_t dimZ = (array3D)[0][0].size();

	for (size_t i = 0; i < dimX; i++)
	{
		for (size_t j = 0; j < dimY; j++)
		{
			for (size_t k = 0; k < dimZ; k++)
				array1D[i + j * dimX + k * dimX * dimY] =  (array3D)[i][j][k];
		}
	}

	temp.close();
	temp.open(filename.toStdString(), 'w');
	temp.writeAllVolumes<float>(array1D);
	temp.close();
	delete array1D;
}

float *Image::arr3Dto1D(vec3df array3D)
{
	float *array1D;
	return array1D;
}

inline bool Image::isFileExists()
{
	struct stat buffer;
	return (stat (filename.toStdString().c_str(), &buffer) == 0);
}
