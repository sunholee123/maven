#ifndef MainWindow_H
#define MainWindow_H

#include <QMainWindow>
#include <QtWidgets>
#include <QImage>
#include <QMouseEvent>
#include <vector>
#include <set>

#include <Eigen/Geometry>
//#include <dcmtk/dcmdata/dctk.h>

#include "Image.h"
#include "LCModelData.h"

#define t1image 0
#define slabimage 1
#define maskimage 2

using namespace Eigen;

struct coord
{
	int a, b, c;
};

class QAction;
class QLabel;
class QMenu;
class QGridLayout;
class QSpinBox;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

private slots:
	void openT1();
	void makeSlabFromDicom();
	void loadT1Segs();
	void openSlab();
	void openLCM();
	void valueUpdateCor(int value);
	void valueUpdateSag(int value);
	void valueUpdateAxi(int value);
	void openMask();
	void makeMaskFromLCM();
	void valueUpdateIntensity(int value);
	void updateMetaChecked(QAbstractButton*);
	void calAvgButtonClicked();
	void calMajorButtonClicked();

private:
	QWidget *mainWidget;
	int planeSize = 500;
	QLabel *plane[3];
	QLabel *sliceInfoText[3];
	QSpinBox *sliceSpinBox[3];
	QTextEdit *outputWindow;
	int sliceNum[3]; // coronal, sagittal, axial slice
	QGridLayout *viewerLayout;
	QGridLayout *ctrlLayout;
	QVBoxLayout *lcmLayout;
	QGroupBox *lcmInfoBox;

	QTextEdit *lcmInfo;

	QLabel *intensityText;
	QSpinBox *intensitySpinBox;

	void createActions();
	void setLCMLayout();

	// menu & actions
	QMenu *slabMenu;
	QMenu *roiMenu;
	QAction *overlaySlabAct;
	QAction *openMaskAct;
	void setEnabledT1DepMenus(bool);

	// Image data
	Image *T1	=	NULL;
	Image *slab	=	NULL;
	Image *mask	=	NULL;

	// DICOM image
	bool findDicomFiles(QString dir);
	Float64 mrsiVoxSizeX;
	Float64 mrsiVoxSizeY;
	Float64 mrsiVoxSizeZ;
	Sint16 mrsiVoxNumX;
	Sint16 mrsiVoxNumY;
	Sint16 mrsiVoxNumZ;

	// LCM info
	QString getLCMFileName();
	float selectedVoxel = 0;

	// Draw and update planes
	void drawPlane(int planeType);
	QImage overlayImage(QImage base, QImage overlay);
	void changeSelectedVoxColor(QImage *img, int planetype, QRgb color);



	// Slab - voxel picking (single voxle selection yet)
	bool eventFilter(QObject *watched, QEvent *e);
	float getSlabVoxelValue(int x, int y, int planeType);
	void changeVoxelValues(float value, bool on);

	bool loadMaskImg(const QString &);
	void voxelQualityCheck(string metabolite, int sd, float fwhm, int snr, int conc, bool pvc);
	void makeMask(string metabolite, bool pvc);
	QString getMaskFileName(string metabolite, int sd, float fwhm, int snr, int conc, bool pvc);


	// statistics
	QStringList selMetaList;
	float calAvgConc(string metabolite);

	// Slab
	void makeSlab();
	QString getSlabFileName();
	bool loadSlabImg(const QString &);
	Image* transformation3d(Image* imagevol, float coordAP, float coordFH, float coordRL, float angleAP, float angleFH, float angleRL, float t1VoxSizeX, float t1VoxSizeY, float t1VoxSizeZ);

	float deg2rad(float degree);
	coord n2abc(int n);

	void print(QString str);
	void printLine();

	void savePref();
	void loadPref();

	QString getPrefFileName();
	int qcMetabolite;
	QString qcSD, qcFWHM, qcSNR, qcConc;
	bool qcPVC;

	// help menu
	void about();

	void setSliceNum(Image *img);
	double intensity;
	void initImgsAll();

	inline bool isFileExists(QString filename);
	void drawPlaneAll();

	// LCModel
	LCModelData *lcm = NULL;
	void presentLCMInfo();
	void saveLCMData();
	void loadLCMData();
	bool loadLCMInfo(QString dir);

	// Select voxel from mask image
	void selectVoxFromMask();
	vector<int> selectedVoxs;
	void saveVoxAsMask();

};

#endif
