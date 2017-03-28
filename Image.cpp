#include "Image.h"

Image::Image()
{
	img = new NiftiImage();
}

Image::Image(QString filename, char rw)
{
	open(filename, rw);
}


Image::~Image()
{
	if (imgvol!=NULL)	{	delete imgvol;	}
	if (img!=NULL)		{	delete img;		}
}

void Image::open(QString filename, char rw)
{
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
		available = true;
	}
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

QString Image::getFileName(QString appstr)
{
	return getFileBaseName() + appstr + "." + getFileExtName();
}


QString Image::getFileBaseName()
{
	return QString::fromStdString(img->basename());
}

QString Image::getFileExtName()
{
	return QString::fromStdString(img->extname());
}

QString Image::getFilePath()
{
	QFileInfo f(filename);
	return f.absolutePath();
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

size_t Image::dx()	{	return dimX;	}
size_t Image::dy()	{	return dimY;	}
size_t Image::dz()	{	return dimZ;	}
float Image::sx()	{	return sizeX;	}
float Image::sy()	{	return sizeY;	}
float Image::sz()	{	return sizeZ;	}

void Image::setDCMInfo(DicomInfo dicominfo)
{
	dcminfo = dicominfo;
}

DicomInfo Image::getDCMInfo()
{
	return dcminfo;
}

void Image::saveImageFile(QString filename, Image* refimg)
{
	string reffilename = refimg->getFileName().toStdString();
	img->open(reffilename, 'r');
	img->close();
	img->open(filename.toStdString(), 'w');
	img->writeAllVolumes<float>(imgvol);
	img->close();
}

inline bool Image::isFileExists()
{
	struct stat buffer;
	return (stat (filename.toStdString().c_str(), &buffer) == 0);
}

bool Image::isAvailable()
{
	return available;
}

QImage Image::getPlaneImage(int planetype, int slicenum)
{
	vector<int> selectedVoxs;
	return getPlaneImage(planetype, slicenum, selectedVoxs);
}

QImage Image::getPlaneImage(int planetype, int slicenum, const vector<int> &selectedVoxs)
{
	QImage planeimage;
	int width = 0;
	int height = 0;
	int width_act = 0;	// need for anisotropy images
	int height_act = 0;	// need for anisotropy images

	switch (planetype)
	{
	case CORONAL:	width = dx();	height = dz();	width_act = width * sx();	height_act = height * sz();	break;
	case SAGITTAL:	width = dy();	height = dz();	width_act = width * sy();	height_act = height * sz();	break;
	case AXIAL:		width = dx();	height = dy();	width_act = width * sx();	height_act = height * sy();	break;
	}

	planeimage = QImage(width, height, QImage::Format_ARGB32);
	planeimage.fill(qRgba(0, 0, 0, 255));

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			float value;
			switch (planetype)
			{
			case CORONAL:	value = getImgVal(i, slicenum, j);	break;
			case SAGITTAL:	value = getImgVal(slicenum, i, j);	break;
			case AXIAL:		value = getImgVal(i, j, slicenum);	break;
			}
			QRgb pixval;
			if (isOverlay())
			{
				// is a selected voxel?
				if (!selectedVoxs.empty() && std::find(selectedVoxs.begin(), selectedVoxs.end(), (int)value) != selectedVoxs.end())
					pixval = qRgba(value * intensity, 0, 0, 255);	// red
				else
					pixval = qRgba(value * intensity, value * intensity, 0, 255);	// yellow
			}
			else
			{
				pixval = qRgba(value * intensity, value * intensity, value * intensity, 255);
			}
			planeimage.setPixel(i, height - j - 1, pixval);

		}
	}

	// need for anisotropy images
	return planeimage.scaled(width_act, height_act, Qt::IgnoreAspectRatio);

}

void Image::setOverlay(bool isoverlay)
{
	overlay = isoverlay;
}

bool Image::isOverlay()
{
	return overlay;
}

void Image::setBlankImgvol(size_t x, size_t y, size_t z)
{
	dimX = x;
	dimY = y;
	dimZ = z;
	size_t n = x * y * z;
	imgvol = new float[n];
}
