#include "MainWindow.h"

/*
Future work
reorientation
*/


MainWindow::MainWindow()
{
	// main widget
	mainWidget = new QWidget();
	mainWidget->setBackgroundRole(QPalette::Dark);
	setCentralWidget(mainWidget);
	QHBoxLayout *mainLayout = new QHBoxLayout();
	mainWidget->setLayout(mainLayout);
	this->move(100, 50);

	for (int i = 0; i < 3; i++)
	{
		plane[i] = new QLabel();
		plane[i]->setFixedWidth(planeSize);
		plane[i]->setFixedHeight(planeSize);
		plane[i]->setAlignment(Qt::AlignHCenter);
		plane[i]->setStyleSheet("background-color: black;");
//        plane[i]->installEventFilter(this);
	}

	viewerLayout = new QGridLayout;
	viewerLayout->addWidget(plane[CORONAL], 0, 0);
	viewerLayout->addWidget(plane[SAGITTAL], 0, 1);
	viewerLayout->addWidget(plane[AXIAL], 1, 0);

	// spinboxes for controlling slices
	for (int i = 0; i < 3; i++)
	{
		sliceSpinBox[i] = new QSpinBox();
		sliceSpinBox[i]->setRange(1, 1);
		sliceNum[i] = 1;
		sliceSpinBox[i]->setValue(sliceNum[i]);
	}

	// intensity spinbox
	intensitySpinBox = new QSpinBox();
	intensitySpinBox->setRange(0, 9999);
	intensity = 0;
	intensitySpinBox->setValue(intensity);
	// output messages window
	outputWindow = new QTextEdit;
	outputWindow->setReadOnly(true);
	connect(sliceSpinBox[0], SIGNAL(valueChanged(int)), this, SLOT(valueUpdateCor(int)));
	connect(sliceSpinBox[1], SIGNAL(valueChanged(int)), this, SLOT(valueUpdateSag(int)));
	connect(sliceSpinBox[2], SIGNAL(valueChanged(int)), this, SLOT(valueUpdateAxi(int)));
	connect(intensitySpinBox, SIGNAL(valueChanged(int)), this, SLOT(valueUpdateIntensity(int)));
	// controllers layout
	ctrlLayout = new QGridLayout;
	sliceInfoText[AXIAL] = new QLabel("<font color='black'>Axial slice:</font>");
	sliceInfoText[SAGITTAL] = new QLabel("<font color='black'>Sagittal slice:</font>");
	sliceInfoText[CORONAL] = new QLabel("<font color ='black'>Coronal slice:</font>");
	int i;

	for (i = 0; i < 3; i++)
	{
		sliceInfoText[i]->setVisible(true);
		ctrlLayout->addWidget(sliceInfoText[i], i, 0);
		ctrlLayout->addWidget(sliceSpinBox[i], i, 1);
	}

	intensityText = new QLabel("<font color = 'black'>Intensity: </font>");
	ctrlLayout->addWidget(intensityText, i, 0);
	ctrlLayout->addWidget(intensitySpinBox, i, 1);
	ctrlLayout->addWidget(outputWindow, i + 1, 0);
	viewerLayout->addLayout(ctrlLayout, 1, 1);
	createActions();
	// LCModel info layout
	lcmLayout = new QVBoxLayout;
	setLCMLayout();
	mainLayout->addLayout(viewerLayout);
	mainLayout->addLayout(lcmLayout);
}

MainWindow::~MainWindow()
{
	if (T1 != NULL)
		delete T1;

	if (slab != NULL)
		delete slab;
}

// not fully implemented : problem with height, add buttons etc.
void MainWindow::setLCMLayout()
{
	if (metaList.isEmpty())
	{
		lcmInfoBox = new QGroupBox(tr("MRSI Chemical Information"));
		lcmInfoBox->setAlignment(Qt::AlignHCenter);
		lcmInfoBox->setFixedWidth(300);
		QLabel *lcmMessage = new QLabel("<font color='black'>Please Load LCM process result files</font>");
		lcmMessage->setAlignment(Qt::AlignCenter);
		QVBoxLayout *vbox = new QVBoxLayout;
		vbox->addWidget(lcmMessage);
		lcmInfoBox->setLayout(vbox);
		lcmLayout->addWidget(lcmInfoBox);
	}
	else if (lcmInfoBox->isVisible())
	{
		// LCM info loaded
		lcmInfoBox->setHidden(true);
		QGroupBox *metabolitesBox = new QGroupBox(tr("select metabolites need to be analyzed"));
		metabolitesBox->setAlignment(Qt::AlignHCenter);
		metabolitesBox->setFixedWidth(300);
		QGridLayout *gbox = new QGridLayout;
		QButtonGroup *group = new QButtonGroup();
		group->setExclusive(false);
		int j = 0;

		for (int i = 0; i < metaList.size(); i++)
		{
			if (i % 2 == 0)
				j += 1;

			QCheckBox *metaBox = new QCheckBox();
			metaBox->setText(metaList[i]);
			gbox->addWidget(metaBox, j, i % 2);
			group->addButton(metaBox);
		}

		connect(group, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(updateMetaChecked(QAbstractButton *)));
		QPushButton *calAvgConButton = new QPushButton("Calculate Avg. Conc.");
		gbox->addWidget(calAvgConButton, j + 1, 0);
		connect(calAvgConButton, SIGNAL(released()), this, SLOT(calAvgButtonClicked()));
		QPushButton *calMajorButton = new QPushButton("Calculate for Major Met.");
		gbox->addWidget(calMajorButton, j + 1, 1);
		connect(calMajorButton, SIGNAL(released()), this, SLOT(calMajorButtonClicked()));
		metabolitesBox->setLayout(gbox);
		lcmInfo = new QTextEdit;
		lcmInfo->setReadOnly(true);
		lcmLayout->addWidget(metabolitesBox);
		lcmLayout->addWidget(lcmInfo);
	}
}

void MainWindow::setEnabledT1DepMenus(bool enabled)
{
	overlaySlabAct->setEnabled(enabled);
	openSlabMaskAct->setEnabled(enabled);
	slabMenu->setEnabled(enabled);
}

/***** widget menu actions *****/
void MainWindow::createActions()
{
	QMenu *fileMenu = menuBar()->addMenu(tr("File"));
	slabMenu = menuBar()->addMenu(tr("Slab"));
	QMenu *helpMenu = menuBar()->addMenu(tr("Help"));

	fileMenu->addAction(tr("Open T1 Image"), this, &MainWindow::openT1);
	overlaySlabAct = fileMenu->addAction(tr("Overlay Slab"), this, &MainWindow::openSlab);
	openSlabMaskAct = fileMenu->addAction(tr("Overlay Slab Mask"), this, &MainWindow::openSlabMask);
	fileMenu->addAction(tr("Exit"), this, &QWidget::close);

	slabMenu->addAction(tr("Create Slab Image from DICOM Files"), this, &MainWindow::makeSlabFromDicom);
	slabMenu->addAction(tr("Load LCM Info"), this, &MainWindow::openLCM);
	slabMenu->addAction(tr("Load FSLVBM Segmented Images"), this, &MainWindow::loadT1Segs);
	slabMenu->addAction(tr("QC + Create Slab Mask Image"), this, &MainWindow::makeSlabMask);
	// Some menus and actions are disabled until the T1 image is fully loaded
	setEnabledT1DepMenus(false);
	// future work: add help action
	fileMenu->addSeparator();
	slabMenu->addSeparator();
	helpMenu->addAction("About", this, &MainWindow::about);
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About"), tr("MAVEN: MRSI Analysis and Visualization ENvironment"));
}

void MainWindow::initImgsAll()
{
	if (T1 != NULL)
	{
		delete T1;
	}
	if (slab != NULL)
	{
		delete slab;
	}
	if (mask != NULL)
	{
		delete mask;
	}

	T1 = new Image();
	slab = new Image();
	slab->setOverlay(true);
	mask = new Image();
}

void MainWindow::openT1()
{
	QFileDialog dialog(this, tr("Open File"));
	dialog.setNameFilter(tr("Nifti files (*.nii.gz)"));
	//	while (dialog.exec() == QDialog::Accepted && !loadImage(dialog.selectedFiles().first()), T1) {}
	if(dialog.exec())
	{
		initImgsAll();
		T1->open(dialog.selectedFiles().first(), 'r');
		print("[Load] T1 image (" + T1->getFileName() + ")");

		if(isFileExists(getPrefFileName()))	{	readPref();						}
		if(isFileExists(getSlabFileName()))	{	loadSlabImg(getSlabFileName());	}
		if(isFileExists(getLCMFileName()))	{	loadLCMData();					}

		setSliceNum(T1);
		setEnabledT1DepMenus(true);

		printLine();


	/*
		if(isFileExists(T1->getFileName()) && slab != NULL)
			loadT1Segs();
	*/
	}
}

void MainWindow::openSlab()
{
	QFileDialog dialog(this, tr("Open File"));
	dialog.setNameFilter(tr("Nifti files (*.nii.gz)"));
	dialog.exec();
	loadSlabImg(dialog.selectedFiles().first());
	drawPlaneAll();
	printLine();
}

void MainWindow::openSlabMask()
{
	QFileDialog dialog(this, tr("Open File"));
	dialog.setNameFilter(tr("Nifti files (*.nii.gz *.nii *.hdr)"));

	while (dialog.exec() == QDialog::Accepted && !loadSlabMask(dialog.selectedFiles().first())) {}

	printLine();
}

void MainWindow::makeSlabFromDicom()
{
	QFileDialog dialog(this, tr("Select Directory"));
	dialog.setFileMode(QFileDialog::Directory);

	if (dialog.exec() == QDialog::Accepted)
	{
		if (findDicomFiles(dialog.selectedFiles().first()))
		{
			makeSlab();
			loadSlabImg(getSlabFileName());
		}
	}
	drawPlaneAll();
	printLine();
}

void MainWindow::loadT1Segs()
{
	// get T1 directory name
	QFileInfo f(T1->getFileName());
	// load gm, wm, csf images
	QString gmFileName = f.absolutePath() + "/struc/" + f.baseName() + "_struc_GM.nii.gz";
	QString wmFileName = f.absolutePath() + "/struc/" + f.baseName() + "_struc_brain_pve_2.nii.gz";
	QString csfFileName = f.absolutePath() + "/struc/" + f.baseName() + "_struc_brain_pve_0.nii.gz";
	// initialize gm, wm and csf values
	struct segvals
	{
		float gm, wm, csf;
	};
	segvals ***s;
	s = new segvals **[mrsiVoxNumZ];

	for (int i = 0; i < mrsiVoxNumZ; i++)
	{
		s[i] = new segvals*[mrsiVoxNumY];

		for (int j = 0; j < mrsiVoxNumY; j++)
		{
			s[i][j] = new segvals[mrsiVoxNumX];

			for (int k = 0; k < mrsiVoxNumX; k++)
			{
				s[i][j][k].gm = 0;
				s[i][j][k].wm = 0;
				s[i][j][k].csf = 0;
			}
		}
	}

	// get segmentation information by voxel
	const size_t dimX = T1->dx();
	const size_t dimY = T1->dy();
	const size_t dimZ = T1->dz();
	Image *gmimg = new Image(gmFileName, 'r');

	for (size_t i = 0; i < dimX; i++)
	{
		for (size_t j = 0; j < dimY; j++)
		{
			for (size_t k = 0; k < dimZ; k++)
			{
				if (slab->getImgVal(i, j, k) != 0)
				{
					coord abc = n2abc(slab->getImgVal(i, j, k));
					s[abc.a][abc.b][abc.c].gm += gmimg->getImgVal(i, j, k);
				}
			}
		}
	}

	delete gmimg;
	Image *wmimg = new Image(wmFileName, 'r');

	for (size_t i = 0; i < dimX; i++)
	{
		for (size_t j = 0; j < dimY; j++)
		{
			for (size_t k = 0; k < dimZ; k++)
			{
				if (slab->getImgVal(i, j, k) != 0)
				{
					coord abc = n2abc(slab->getImgVal(i, j, k));
					s[abc.a][abc.b][abc.c].wm += wmimg->getImgVal(i, j, k);
				}
			}
		}
	}

	delete wmimg;
	Image *csfimg = new Image(csfFileName, 'r');

	for (size_t i = 0; i < dimX; i++)
	{
		for (size_t j = 0; j < dimY; j++)
		{
			for (size_t k = 0; k < dimZ; k++)
			{
				if (slab->getImgVal(i, j, k) != 0)
				{
					coord abc = n2abc(slab->getImgVal(i, j, k));
					s[abc.a][abc.b][abc.c].csf += csfimg->getImgVal(i, j, k);
				}
			}
		}
	}

	delete csfimg;
	print("[Load] GM (" + gmFileName + ")");
	print("[Load] WM (" + wmFileName + ")");
	print("[Load] CSF (" + csfFileName + ")");

	// calculate PVC value automatically
	// calculate volume fractions
	float mrsiVoxVolume = mrsiVoxSizeX * mrsiVoxSizeY * mrsiVoxSizeZ;

	for (int i = 0; i < mrsiVoxNumZ; i++)
	{
		for (int j = 0; j < mrsiVoxNumY; j++)
		{
			for (int k = 0; k < mrsiVoxNumX; k++)
			{
				float total = s[i][j][k].gm + s[i][j][k].wm + s[i][j][k].csf;

				// qc: pvc = 0 if an mrsi voxel is not included in the brain
				if (total < (mrsiVoxVolume * 0.8))	// 80%
				{
					tables[i][j][k].isAvailable = false;
					continue;
				}

				float f_gm = s[i][j][k].gm / total;
				float f_wm = s[i][j][k].wm / total;
				float f_csf = s[i][j][k].csf / total;
				// calculate partial volume corection values
				tables[i][j][k].pvc = (43300 * f_gm + 35880 * f_wm + 55556 * f_csf) / ((1 - f_csf) * 35880);
			}
		}
	}

	print("[Info] Partial Volume Correction is complete.");
	printLine();
}

void MainWindow::makeSlabMask()
{
	// future work: if no LCM data loaded, then popup message
	QDialog dialog(this);
	QFormLayout form(&dialog);
	form.addRow(new QLabel("<center>Values for Quality Check</center>"));
	QComboBox *metabolites = new QComboBox();
	metabolites->addItems(metaList);
	form.addRow("Metabolite", metabolites);
	QLineEdit *sdInput = new QLineEdit(&dialog);
	sdInput->setValidator(new QIntValidator(0, 100, this));
	sdInput->setText("20");
	form.addRow("SD(%)", sdInput);
	QLineEdit *fwhmInput = new QLineEdit(&dialog);
	fwhmInput->setValidator(new QDoubleValidator(0, 10, 2, this));
	fwhmInput->setText("0.2");
	form.addRow("FWHM", fwhmInput);
	QLineEdit *snrInput = new QLineEdit(&dialog);
	snrInput->setValidator(new QIntValidator(0, 10, this));
	form.addRow("SNR(optional)", snrInput);
	QLineEdit *concInput = new QLineEdit(&dialog);
	snrInput->setValidator(new QIntValidator(0, 10000, this));
	form.addRow("Conc(optional)", concInput);
	QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
	form.addRow(&buttonBox);
	QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
	QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

	if (dialog.exec() == QDialog::Accepted)
	{
		string metabolite = metabolites->currentText().toStdString();
		int sd = sdInput->text().toInt();
		float fwhm = fwhmInput->text().toFloat();
		int snr = -1;
		int conc = 20000;

		if (!snrInput->text().isEmpty())
			snr = snrInput->text().toInt();

		if (!concInput->text().isEmpty())
			conc = concInput->text().toInt();

		voxelQualityCheck(metabolite, sd, fwhm, snr, conc);
		saveSlabMask(metabolite);
		loadSlabMask(getMaskFileName(metabolite));
	}

	printLine();
}

void MainWindow::openLCM()
{
	QFileDialog dialog(this, tr("Select Directory"));
	dialog.setFileMode(QFileDialog::Directory);

	if (dialog.exec() == QDialog::Accepted && loadLCMInfo(dialog.selectedFiles().first()))
		saveLCMData();

	printLine();
}

void MainWindow::setSliceNum(Image *img)
{
	int maxdim[3];
	maxdim[CORONAL] = img->dy();
	maxdim[SAGITTAL] = img->dx();
	maxdim[AXIAL] = img->dz();

	for (int i = 0; i < 3; i++)
	{
		sliceSpinBox[i]->setRange(1, maxdim[i]);
		sliceSpinBox[i]->setValue(maxdim[i] / 2);
	}
}

/***** load Dicom image *****/
bool MainWindow::findDicomFiles(QString dir)
{
	QDirIterator it(dir, QDir::Files, QDirIterator::Subdirectories);

	if (!it.hasNext())
	{
		QMessageBox::critical(this, "Error!", "Can't find DICOM files.", QMessageBox::Ok);
		return false;
	}

	DcmFileFormat fileformat;
	OFCondition status;
	OFString seriesDescription;
	DcmSequenceOfItems *sqi;
	DcmItem *item;
	bool T1flag = false;
	bool MRSIflag = false;

	while (it.hasNext())
	{
		status = fileformat.loadFile(it.next().toStdString().c_str());

		if (status.good())
		{
			if (fileformat.getDataset()->findAndGetSequence(DcmTag(0x2001, 0x105f, "Philips Imaging DD 001"), sqi, false, false).good()
					&& fileformat.getDataset()->findAndGetOFString(DCM_SeriesDescription, seriesDescription).good())
			{
				if (!T1flag && (seriesDescription.compare("T1_SAG_MPRAGE_1mm_ISO") == 0 || seriesDescription.compare("3D T1 TFE SAG") == 0))
					// 3D t1 TFE SAG: for non-human primates
				{
					item = sqi->getItem(0);
					DicomInfo T1dcminfo;
					item->findAndGetFloat32(DcmTag(0x2005, 0x1078), T1dcminfo.coordAP, 0, false).good();
					item->findAndGetFloat32(DcmTag(0x2005, 0x1079), T1dcminfo.coordFH, 0, false).good();
					item->findAndGetFloat32(DcmTag(0x2005, 0x107a), T1dcminfo.coordRL, 0, false).good();
					item->findAndGetFloat32(DcmTag(0x2005, 0x1071), T1dcminfo.angleAP, 0, false).good();
					item->findAndGetFloat32(DcmTag(0x2005, 0x1072), T1dcminfo.angleFH, 0, false).good();
					item->findAndGetFloat32(DcmTag(0x2005, 0x1073), T1dcminfo.angleRL, 0, false).good();
					T1->setDCMInfo(T1dcminfo);
					T1flag = true;
				}
				//else if (!MRSIflag && seriesDescription.compare("3SL_SECSI_TE19") == 0)
				else if (!MRSIflag && seriesDescription.find("3SL_SECSI_TE19", 0) != string::npos)
				{
					bool success = true;
					item = sqi->getItem(0);
					DcmDataset *dset = fileformat.getDataset();
					// Pixel spacing, slice thickness
					success *= dset->findAndGetFloat64(DcmTag(0x0028, 0x0030), mrsiVoxSizeX, 0, false).good(); // Pixel Spacing 0
					success *= dset->findAndGetFloat64(DcmTag(0x0028, 0x0030), mrsiVoxSizeY, 1, false).good(); // Pixel Spacing 1
					success *= dset->findAndGetFloat64(DcmTag(0x0018, 0x0050), mrsiVoxSizeZ, 0, false).good(); // Slice Thickness
					// Coordinates
					DicomInfo MRSIdcminfo;
					success *= item->findAndGetFloat32(DcmTag(0x2005, 0x1078), MRSIdcminfo.coordAP, 0, false).good();
					success *= item->findAndGetFloat32(DcmTag(0x2005, 0x1079), MRSIdcminfo.coordFH, 0, false).good();
					success *= item->findAndGetFloat32(DcmTag(0x2005, 0x107a), MRSIdcminfo.coordRL, 0, false).good();
					success *= item->findAndGetFloat32(DcmTag(0x2005, 0x1071), MRSIdcminfo.angleAP, 0, false).good();
					success *= item->findAndGetFloat32(DcmTag(0x2005, 0x1072), MRSIdcminfo.angleFH, 0, false).good();
					success *= item->findAndGetFloat32(DcmTag(0x2005, 0x1073), MRSIdcminfo.angleRL, 0, false).good();
					slab->setDCMInfo(MRSIdcminfo);
					// The number of MRSI slab voxels
					fileformat.getDataset()->findAndGetSequence(DcmTag(0x2005, 0x1371, "Philips Imaging DD 004"), sqi, false, false);
					item = sqi->getItem(0);
					success *= item->findAndGetSint16(DcmTag(0x2005, 0x1376), mrsiVoxNumX, 0, false).good();
					success *= item->findAndGetSint16(DcmTag(0x2005, 0x1377), mrsiVoxNumY, 0, false).good();
					success *= item->findAndGetSint16(DcmTag(0x2005, 0x1378), mrsiVoxNumZ, 0, false).good();

					if (mrsiVoxNumX * mrsiVoxNumY > 0)
						mrsiVoxNumZ = mrsiVoxNumZ / (mrsiVoxNumX * mrsiVoxNumY);

					if (success)
						MRSIflag = true;
				}

				if (T1flag && MRSIflag)
				{
					savePref(); // save mrsivoxnum values
					readPref();
					return true;
				}
			}
		}
		else
			it.next();
	}

	return false;
}

/***** load Slab image *****/
bool MainWindow::loadSlabImg(const QString &fileName)
{
	delete slab;
	slab = new Image();
	slab->open(fileName, 'r');
	slab->setOverlay(true);
	print("[Load] Slab image (" + fileName + ")");
	return true;
}

void MainWindow::drawPlaneAll()
{
	drawPlane(CORONAL);
	drawPlane(SAGITTAL);
	drawPlane(AXIAL);
}

/***** load LCM info *****/
bool MainWindow::loadLCMInfo(QString dir)
{
	QDirIterator it(dir, QStringList() << "*.table", QDir::Files, QDirIterator::Subdirectories);

	if (!it.hasNext())
	{
		QMessageBox::critical(this, "Error!", "Can't find *.table files.", QMessageBox::Ok);
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

	setLCMLayout();
	print("[Load] LCModel table files (" + dir + ", " + QString::number(filecount) + " files)");
	return true;
}

TableInfo MainWindow::parseTable(string filename)
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

void MainWindow::presentLCMInfo()
{
	QString info_str;
	map<string, Metabolite>::iterator metaPos;
	coord abc = n2abc(selectedVoxel);
	TableInfo temp = tables[abc.a][abc.b][abc.c];
	info_str.append("<qt><style>.mytable{ border-collapse:collapse; }");
	info_str.append(".mytable th, .mytable td { border:5px solid black; }</style>");
	info_str.append("<table class=\"mytable\"><tr><th>Metabolite</th><th>Conc.</th><th>%SD</th><th>/Cr</th></tr>");

	for (metaPos = temp.metaInfo.begin(); metaPos != temp.metaInfo.end(); ++metaPos)
	{
		string s1 = "<tr><td>" + metaPos->first + "</td>";
		string s2 = "<td>" + to_string(metaPos->second.conc) + "</td>";
		string s3 = "<td>" + to_string(metaPos->second.sd) + "</td>";
		string s4 = "<td>" + to_string(metaPos->second.ratio) + "</td></tr>";
		info_str.append(QString::fromStdString(s1 + s2 + s3 + s4));
	}

	/*
	for (int i = 0; i < 35; i++) {
		string s1 = "<tr><td>" + tables[a - 1][b - 1][c - 1].metaInfo[i][3] + "</td>";
		string s2 = "<td>" + tables[a - 1][b - 1][c - 1].metaInfo[i][0] + "</td>";
		string s3 = "<td>" + tables[a - 1][b - 1][c - 1].metaInfo[i][1] + "</td>";
		string s4 = "<td>" + tables[a - 1][b - 1][c - 1].metaInfo[i][2] + "</td></tr>";
		info_str.append(QString::fromStdString(s1 + s2 + s3 + s4));
	}
	*/
	info_str.append("</table></qt>");
	lcmInfo->setText(QString::fromStdString("Selected: Slice " + to_string(abc.a + 1) + ", Row " + to_string(abc.b + 1) + ", Col " + to_string(abc.c + 1) + " (" + to_string((int)selectedVoxel) + ")"));
	lcmInfo->append(info_str);
	lcmInfo->append("\n\nFWHM: " + QString::number(temp.fwhm));
	lcmInfo->append("SNR: " + QString::number(temp.snr));
}


/***** draw and update planes *****/

void MainWindow::drawPlane(int planetype)
{
	QImage baseimg, overlayimg;
	int slice = sliceNum[planetype];
	// set images
	if (T1->isAvailable())
	{
		baseimg = T1->getPlaneImage(planetype, slice);
	}
	if (slab->isAvailable())
	{
		overlayimg = slab->getPlaneImage(planetype, slice);
		baseimg = overlayImage(baseimg, overlayimg);
	}

	// draw part
	plane[planetype]->setPixmap(QPixmap::fromImage(baseimg.scaled(planeSize, planeSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
}

QImage MainWindow::overlayImage(QImage base, QImage overlay)
{
	QImage result(base.width(), base.height(), QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&result);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(result.rect(), Qt::transparent);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawImage(0, 0, base);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.setOpacity(0.5);
	painter.drawImage(0, 0, overlay);
	painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
	painter.fillRect(result.rect(), Qt::white);
	painter.end();
	return result;
}

void MainWindow::valueUpdateCor(int value)
{
	sliceNum[CORONAL] = value - 1;
	drawPlane(CORONAL);
}

void MainWindow::valueUpdateSag(int value)
{
	sliceNum[SAGITTAL] = value - 1;
	drawPlane(SAGITTAL);
}

void MainWindow::valueUpdateAxi(int value)
{
	sliceNum[AXIAL] = value - 1;
	drawPlane(AXIAL);
}

void MainWindow::valueUpdateIntensity(int value)
{
	intensity = 300.0 / value;
	drawPlaneAll();
}

/***** slab voxel picking *****/

bool MainWindow::eventFilter(QObject *watched, QEvent *e)
{/*
	if (e->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent *event = (QMouseEvent *)e;

		if (event->button() == Qt::LeftButton)
		{
			if (overlay)   // get mouse event only when slab overlayed
			{
				if ((QLabel *)watched == plane[CORONAL] || (QLabel *)watched == plane[SAGITTAL] || (QLabel *)watched == plane[AXIAL]) // and mouse event occured in plane
				{
					int x = event->x();
					int y = event->y();
					int planeType;
					float val;

					if ((QLabel *)watched == plane[CORONAL])
						planeType = CORONAL;
					else if ((QLabel *)watched == plane[SAGITTAL])
						planeType = SAGITTAL;
					else
						planeType = AXIAL;

					int width1 = slabplanes[planeType].width();
					int height1 = slabplanes[planeType].height();
					int width2 = slabplanes[planeType].scaled(planeSize, planeSize, Qt::KeepAspectRatio, Qt::SmoothTransformation).width();
					int height2 = slabplanes[planeType].scaled(planeSize, planeSize, Qt::KeepAspectRatio, Qt::SmoothTransformation).height();
					int margin1 = 0;
					int margin2 = 0;

					if (width2 < planeSize)
						margin1 = (planeSize - width2) / 2;

					if (height2 < planeSize)
						margin2 = (planeSize - height2) / 2;

					int a = (x - margin1) * width1 / width2;
					int b = (y - margin2) * height1 / height2;

					if ((a > 0) && (a < width1) && (b > 0) && (b < height1))
					{
						val = getSlabVoxelValue(a, b, planeType);

						if (selectedVoxel == val)
							selectedVoxel = 0;
						else
							selectedVoxel = val;
					}
				}
			}
		}
	}
	else if (e->type() == QEvent::MouseButtonRelease)
	{
		if (overlay)   // get mouse event only when slab overlayed
		{
			if ((QLabel *)watched == plane[CORONAL] || (QLabel *)watched == plane[SAGITTAL] || (QLabel *)watched == plane[AXIAL]) // and mouse event occured in plane
			{
				drawImg2Plane(CORONAL);
				drawImg2Plane(SAGITTAL);
				drawImg2Plane(AXIAL);

				if (tables != NULL && selectedVoxel)
					presentLCMInfo();
			}
		}
	}

	return false;*/
}


float MainWindow::getSlabVoxelValue(int x, int y, int planetype)
{
	/*
//    int width = slabImages[planeType].width();
	int height = slabplanes[planetype].height();
	float val;

	switch (planetype)
	{
	case CORONAL:
		val = slab->getImgVal(x, sliceNum[planetype], height - y);
		break;

	case SAGITTAL:
		val = slab->getImgVal(sliceNum[planetype], x, height - y);
		break;

	case AXIAL:
		val = slab->getImgVal(x, height - y, sliceNum[planetype]);
		break;

	default:
		val = -1;
	}

	return val;
	*/
}

void MainWindow::changeVoxelValues(float value, bool on)
{/*
	if (value != -1)   // change value only for slab voxel
	{
		float temp;

		for (int p = 0; p < 3; p++)   // update voxel value for every plane
		{
			for (int i = 0; i < slabplanes[p].width(); i++)
			{
				for (int j = 0; j < slabplanes[p].height(); j++)
				{
					if (p == CORONAL)
						temp = slab->getImgVal(i, sliceNum[p], j);
					else if (p == SAGITTAL)
						temp = slab->getImgVal(sliceNum[p], i, j);
					else if (p == AXIAL)
						temp = slab->getImgVal(i, j, sliceNum[p]);

					if (temp == value)
					{
						if (on == true)
							slabplanes[p].setPixelColor(i, slabplanes[p].height() - j, qRgba(temp * intensity, 0, 0, 255));
						else
							slabplanes[p].setPixelColor(i, slabplanes[p].height() - j, qRgba(temp * intensity, temp * intensity, 0, 255));
					}
				}
			}
		}
	}*/
}

/***** slab mask making (voxel quality check) *****/
bool MainWindow::loadSlabMask(const QString &fileName)
{
	mask->open(fileName, 'r');

	drawPlaneAll();
	return true;
}

void MainWindow::voxelQualityCheck(string metabolite, int sd, float fwhm, int snr, int conc)
{
	if (sd == -1 || fwhm == -1)
	{
		// exception -- not available sd, fwhm values
	}
	else if (tables == NULL)
	{
		// exception -- LCM info did not load
	}
	else
	{
		for (int i = 0; i < mrsiVoxNumZ; i++)
		{
			for (int j = 0; j < mrsiVoxNumY; j++)
			{
				for (int k = 0; k < mrsiVoxNumX; k++)
				{
					if (tables[i][j][k].isAvailable)
					{
						map<string, Metabolite>::iterator tempPos;
						tempPos = tables[i][j][k].metaInfo.find(metabolite);

						if (tempPos != tables[i][j][k].metaInfo.end())
						{
							if (tempPos->second.sd > sd || tempPos->second.conc > conc || tables[i][j][k].fwhm > fwhm || tables[i][j][k].snr < snr)
								tempPos->second.qc = false;
							else
								tempPos->second.qc = true;
						}
						else
						{
							// exception -- metabolite not found
						}
					}
				}
			}
		}

		//lcmInfo->append("slab table qc value all changed");
		print("[Info] All QC values of slab voxels are changed.");
	}
}

void MainWindow::saveSlabMask(string metabolite)
{
	/*
	if (slab->getImgVal().empty())
	{
		lcmInfo->setText("slab is empty, cannot init mask image size");
	}
	*/
	for (size_t i = 0; i < slab->dx(); i++)
	{
		for (size_t j = 0; j < slab->dy(); j++)
		{
			for (size_t k = 0; k < slab->dz(); k++)
			{
				if (slab->getImgVal(i, j, k) != 0)
				{
					coord abc = n2abc(slab->getImgVal(i, j, k));

					if (tables[abc.a][abc.b][abc.c].fwhm == -1)
						mask->setImgVal(i, j, k, 0);
					else
					{
						map<string, Metabolite>::iterator tempPos = tables[abc.a][abc.b][abc.c].metaInfo.find(metabolite);

						if (tempPos != tables[abc.a][abc.b][abc.c].metaInfo.end())
						{
							if (tempPos->second.qc && tables[abc.a][abc.b][abc.c].isAvailable)
							{
								//imagevol[i][j][k] = 1;
								mask->setImgVal(i, j, k, tempPos->second.conc);
								//gpeimagevol[i][j][k] = tempPos->second.conc * tables[abc.a][abc.b][abc.c].pvc;
							}
							else
								mask->setImgVal(i, j, k, 0);
						}
					}
				}
			}
		}
	}

//	mask->saveImageFile(getMaskFileName(metabolite));
	print("[Save] Slab mask image");
}

/***** statistics *****/
void MainWindow::updateMetaChecked(QAbstractButton *button)
{
	if (button->isChecked())
	{
		//lcmInfo->append(button->text());
		selMetaList.push_back(button->text());
	}
	else
	{
		//lcmInfo->append("unchecked");
		int index = selMetaList.indexOf(button->text(), 0);
		selMetaList.removeAt(index);
	}
}

void MainWindow::calAvgButtonClicked()
{
	if (selMetaList.empty())
		lcmInfo->setText("Please select metabolites");
	else
	{
		QString infotext = "Average concentration of selected metabolites\n";

		for (int i = 0; i < selMetaList.size(); i++)
		{
			string metabolite = selMetaList[i].toStdString();
			float avg = calAvgConc(metabolite);
			string text = metabolite + ": " + std::to_string(avg) + "\n";
			infotext.append(QString::fromStdString(text));
		}

		lcmInfo->setText(infotext);
	}
}

float MainWindow::calAvgConc(string metabolite)
{
	float sum = 0;
	int count = 0;

	for (int i = 0; i < mrsiVoxNumZ; i++)
	{
		for (int j = 0; j < mrsiVoxNumY; j++)
		{
			for (int k = 0; k < mrsiVoxNumX; k++)
			{
				if (tables[i][j][k].isAvailable)
				{
					map<string, Metabolite>::iterator tempPos;
					tempPos = tables[i][j][k].metaInfo.find(metabolite);

					if (tempPos != tables[i][j][k].metaInfo.end())
					{
						if (tempPos->second.qc)
						{
							count++;
							sum += tempPos->second.conc * tables[i][j][k].pvc;
						}
					}
				}
			}
		}
	}

	string s1 = "sum: " + std::to_string(sum);
	string s2 = "count: " + std::to_string(count);
	lcmInfo->append(QString::fromStdString(s1));
	lcmInfo->append(QString::fromStdString(s2));
	return (sum / count);
}

void MainWindow::calMajorButtonClicked()
{
	QStringList majorList = { "NAA", "GPC+PCh", "Cr+PCr", "Glu+Gln", "Ins" };
	int sd = 20;
	float fwhm = 0.2;
	int snr = -1;
	int conc = 50;
	QString titletext = "Average concentration of major metabolites\n";
	//titletext.append("QC values: %SD<=20, FWHM<=0.1\n");
	titletext.append("QC values: %SD<=20, FWHM<=0.2, Conc<=50\n");
	//titletext.append("QC values: %SD<=20, FWHM<=0.15\n");
	lcmInfo->setText(titletext);

	for (int i = 0; i < majorList.size(); i++)
	{
		string metabolite = majorList[i].toStdString();
		voxelQualityCheck(metabolite, sd, fwhm, snr, conc);
		float avg = calAvgConc(metabolite);
		string text = metabolite + ": " + std::to_string(avg) + "\n";
		lcmInfo->append(QString::fromStdString(text));
	}
}

Image* MainWindow::transformation3d(Image* imagevol, float coordAP, float coordFH, float coordRL, float angleAP, float angleFH, float angleRL, float t1VoxSizeX, float t1VoxSizeY, float t1VoxSizeZ)
{
	const size_t dimX = imagevol->dx();
	const size_t dimY = imagevol->dy();
	const size_t dimZ = imagevol->dz();
	// transformation vector calculation
	Affine3f rotRL = Affine3f(AngleAxisf(deg2rad(angleRL), Vector3f(-1, 0, 0))); // sagittal - clockwise
	Vector3f u1 = rotRL * Vector3f(0, 1, 0);
	Affine3f rotAP = Affine3f(AngleAxisf(deg2rad(angleAP), u1));	// coronal - clockwise
	Vector3f u2 = rotAP * Vector3f(0, 0, 1);
	Affine3f rotFH = Affine3f(AngleAxisf(deg2rad(angleFH), u2));	// axial - counterclockwise
	Affine3f r = rotFH * rotAP * rotRL;
	Affine3f t1(Translation3f(Vector3f(-round(dimX / 2), -round(dimY / 2), -round(dimZ / 2))));
	Affine3f t2(Translation3f(Vector3f(round(dimX / 2), round(dimY / 2), round(dimZ / 2))));
	// 1st param++ ==> slab moves to left
	// 2nd param++ ==> slab moves to front
	// 3rd param++ ==> slab moves to up
	Affine3f t3(Translation3f(Vector3f(coordRL / t1VoxSizeX, -coordAP / t1VoxSizeY, coordFH / t1VoxSizeZ)));
	Matrix4f m = (t3 * t2 * r * t1).matrix();
	Matrix4f m_inv = m.inverse();

	Image *rotvol = new Image();
	rotvol->setBlankImgvol(dimX, dimY, dimZ);

	QTime myTimer;
	myTimer.start();

	for (size_t i = 0; i < dimX; i++)
	{
		for (size_t j = 0; j < dimY; j++)
		{
			for (size_t k = 0; k < dimZ; k++)
			{
				Vector4f newcoord(i, j, k, 1);
				Vector4f imgcoord = m_inv * newcoord;

				if ((imgcoord(0) >= 0) && (imgcoord(1) >= 0) && (imgcoord(2) >= 0))
				{
					size_t x = round(imgcoord(0));
					size_t y = round(imgcoord(1));
					size_t z = round(imgcoord(2));

					if ((x < dimX) && (y < dimY) && (z < dimZ))
						rotvol->setImgVal(i, j, k, imagevol->getImgVal(x, y, z));
				}
				else
					rotvol->setImgVal(i, j, k, 0);
			}
		}
	}
	int nMilliseconds = myTimer.elapsed();
	print("[Time] " + QString::number(nMilliseconds) + "ms");

	return rotvol;
}

float MainWindow::deg2rad(float degree)
{
	return degree * M_PI / 180;
}

void MainWindow::makeSlab()
{
	float t1VoxSizeX = T1->sx();
	float t1VoxSizeY = T1->sy();
	float t1VoxSizeZ = T1->sz();
	float defSlabSizeX = mrsiVoxSizeX * mrsiVoxNumX / t1VoxSizeX;
	float defSlabSizeY = mrsiVoxSizeY * mrsiVoxNumY / t1VoxSizeY;
	float defSlabSizeZ = mrsiVoxSizeZ * mrsiVoxNumZ / t1VoxSizeZ;
	int maxLength = round(pow(pow(defSlabSizeX, 2) + pow(defSlabSizeY, 2) + pow(defSlabSizeZ, 2), 0.5));
	int slabMid = maxLength / 2;
	int slabX = round(defSlabSizeX / 2);
	int slabY = round(defSlabSizeY / 2);
	int slabZ = round(defSlabSizeZ / 2);
	int slabdiffX = slabMid - slabX;
	int slabdiffY = slabMid - slabY;
	int slabdiffZ = slabMid - slabZ;

	Image *imagevol = new Image();
	imagevol->setBlankImgvol(maxLength, maxLength, maxLength);


	for (int i = slabdiffX; i < slabMid + slabX; i++)
	{
		for (int j = slabdiffY; j < slabMid + slabY; j++)
		{
			for (int k = slabdiffZ; k < slabMid + slabZ - 1; k++) // mrsiVoxNumZ°¡ ÈŠŒö¶óŒ­ -1 Ãß°¡...
			{
				int voxNumX = mrsiVoxNumX + 1 - round((i - slabdiffX) * t1VoxSizeX / mrsiVoxSizeX + 0.5); // left to right
				int voxNumY = mrsiVoxNumY + 1 - round((j - slabdiffY) * t1VoxSizeY / mrsiVoxSizeY + 0.5);
				int voxNumZ = round((k - slabdiffZ) * t1VoxSizeZ / mrsiVoxSizeZ + 0.5); // bottom to top
				int value = voxNumX + (voxNumY - 1) * mrsiVoxNumX + (voxNumZ - 1) * mrsiVoxNumX * mrsiVoxNumY;
				imagevol->setImgVal(i, j, k, value);
			}
		}
	}

	// rotate and translate
	DicomInfo diff = slab->getDCMInfo() - T1->getDCMInfo();
	Image *imagevol2 = transformation3d(imagevol, diff.coordAP, diff.coordFH, diff.coordRL, diff.angleAP, diff.angleFH, diff.angleRL, t1VoxSizeX, t1VoxSizeY, t1VoxSizeZ);
	delete imagevol;

	// slab image (full size)
	Image *slabvol = new Image();
	const size_t dimX = T1->dx();
	const size_t dimY = T1->dy();
	const size_t dimZ = T1->dz();
	slabvol->setBlankImgvol(dimX, dimY, dimZ);

	// crop
	int diffX = slabMid - round(dimX / 2);
	int diffY = slabMid - round(dimY / 2);
	int diffZ = slabMid - round(dimZ / 2);

	for (int i = 0; i < (int)dimX; i++)
	{
		for (int j = 0; j < (int)dimY; j++)
		{
			for (int k = 0; k < (int)dimZ; k++)
			{
				if ((i + diffX) < 0 || (j + diffY) < 0 || (k + diffZ) < 0 || (i + diffX) >= maxLength || (j + diffY) >= maxLength || (k + diffZ) >= maxLength)
					slabvol->setImgVal(i, j, k, 0);
				else
					slabvol->setImgVal(i, j, k, imagevol2->getImgVal(i + diffX, j + diffY, k + diffZ));
			}
		}
	}

	delete imagevol2;
	print("[Info] Slab image is created.");
	QString filename = getSlabFileName();
	slabvol->saveImageFile(filename, T1->getFileName());
	delete slabvol;
	print("[Save] Slab image (" + filename + ")");

}


coord MainWindow::n2abc(int n)
{
	coord temp;
	temp.a = (n - 1) / (mrsiVoxNumX * mrsiVoxNumY);
	temp.b = fmod((n - 1), (mrsiVoxNumX * mrsiVoxNumY)) / mrsiVoxNumY;
	temp.c = fmod(fmod((n - 1), 1024), mrsiVoxNumX);
	return temp;
}

void MainWindow::saveLCMData()
{
	QString filename = getLCMFileName();
	QFile file(filename);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		/*
		   QMessageBox::StandardButton msg;
		   msg = QMessageBox::critical(this, "Error!", "LCM File Creation Failed.", QMessageBox::Ok);
		   */
		QMessageBox::critical(this, "Error!", "LCM File Creation Failed.", QMessageBox::Ok);
		return;
	}

	QTextStream out(&file);

	for (int i = 0; i < mrsiVoxNumZ; i++)
	{
		for (int j = 0; j < mrsiVoxNumY; j++)
		{
			for (int k = 0; k < mrsiVoxNumX; k++)
			{
				// need to optimize?
				out << (i + 1) << "\t" << (j + 1) << "\t" << (k + 1) << "\t";

				if (tables[i][j][k].isAvailable)
				{
					string str = "";
					map<string, Metabolite>::iterator metaPos;

					for (metaPos = tables[i][j][k].metaInfo.begin(); metaPos != tables[i][j][k].metaInfo.end(); metaPos++)
					{
						out << QString::fromStdString(metaPos->second.name) << "\t"
							<< metaPos->second.conc << "\t"
							<< metaPos->second.sd << "\t"
							<< metaPos->second.ratio << "\t";
					}

					out << tables[i][j][k].fwhm << "\t" << tables[i][j][k].snr << "\n";
				}
				else
					out << "-1\n";
			}
		}
	}

	print("[Save] LCModel data (" + filename + ")");
}

void MainWindow::loadLCMData()
{
	QString filename = getLCMFileName();
	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox::critical(this, "Error!", "LCM File Open Failed.", QMessageBox::Ok);
		return;
	}

	QTextStream in(&file);

	if (tables == NULL)
	{
		tables = new TableInfo **[mrsiVoxNumZ];

		for (int i = 0; i < mrsiVoxNumZ; i++)
		{
			tables[i] = new TableInfo*[mrsiVoxNumY];

			for (int j = 0; j < mrsiVoxNumY; j++)
				tables[i][j] = new TableInfo[mrsiVoxNumX];
		}
	}

	while (!in.atEnd())
	{
		QString tempstr = in.readLine();
		QStringList tokens;
		tokens = tempstr.split("\t");
		int a, b, c;
		a = tokens[0].toInt() - 1;
		b = tokens[1].toInt() - 1;
		c = tokens[2].toInt() - 1;

		if (tokens[3] == "-1")
			tables[a][b][c].isAvailable = false;
		else
		{
			tables[a][b][c].isAvailable = true;
			int tokenLen = tokens.length();
			int metaSize = (tokenLen - 3 - 2) / 4;

			for (int i = 0; i < metaSize; i++)
			{
				Metabolite metainfo;
				metainfo.name = tokens[3 + i * 4].toStdString();
				metainfo.conc = tokens[4 + i * 4].toFloat();
				metainfo.sd = tokens[5 + i * 4].toInt();
				metainfo.ratio = tokens[6 + i * 4].toFloat();
				metainfo.qc = true;
				tables[a][b][c].metaInfo[metainfo.name] = metainfo;

				if (metaList.empty() || !metaList.contains(QString::fromStdString(metainfo.name)))   // To-do: call routine just once
					metaList.push_back(QString::fromStdString(metainfo.name));
			}

			tables[a][b][c].fwhm = tokens[tokenLen - 2].toFloat();
			tables[a][b][c].snr = tokens[tokenLen - 1].toInt();
		}
	}

//	saveLCMData();	// test for equality
	print("[Load] LCModel data (" + filename + ")");
	setLCMLayout();
}


QString MainWindow::getSlabFileName()	{	return T1->getFileName("_slab");				}
QString MainWindow::getLCMFileName()	{	return T1->getFileBaseName() + ".lcm";			}
QString MainWindow::getPrefFileName()	{	return T1->getFilePath() + "/maven_info.txt";	}
QString MainWindow::getMaskFileName(string metabolite)
{
	return T1->getFileName("_mask_"+ QString::fromStdString(metabolite));
}

void MainWindow::print(QString str)
{
	outputWindow->append(str);
}

void MainWindow::printLine()
{
	outputWindow->append("--------------------------------------------------");
}


void MainWindow::savePref()
{
	QString filename = getPrefFileName();
	QFile file(filename);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QMessageBox::critical(this, "Error!", "Prefernces File Creation Failed.", QMessageBox::Ok);
		return;
	}

	QTextStream out(&file);
	out << mrsiVoxNumX << " " << mrsiVoxNumY << " " << mrsiVoxNumZ << " "
		<< mrsiVoxSizeX << " " << mrsiVoxSizeY << " " << mrsiVoxSizeZ;
	print("[Save] Preferences file (" + filename + ")");
}

void MainWindow::readPref()
{
	QString filename = getPrefFileName();
	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox::critical(this, "Error!", "LCM File Open Failed.", QMessageBox::Ok);
		return;
	}

	QTextStream in(&file);
	in >> mrsiVoxNumX >> mrsiVoxNumY >> mrsiVoxNumZ
	   >> mrsiVoxSizeX >> mrsiVoxSizeY >> mrsiVoxSizeZ;
	print("[Load] Preferences file (" + filename + ")");
}

inline bool MainWindow::isFileExists(QString filename)
{
	struct stat buffer;
	return (stat (filename.toStdString().c_str(), &buffer) == 0);
}
