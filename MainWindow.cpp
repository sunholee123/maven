#include "MainWindow.h"

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
//	if (lcm->metaList.isEmpty())
	if (lcm == NULL)
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


		QSettings settings(T1->getFilePath()+"/maven_settings.txt", QSettings::NativeFormat);
		QString metalist_temp = settings.value("metalist").toString();
		QStringList metalist_avail = metalist_temp.split(" ");
		//QStringList metalist_avail = settings.value("metalist").toStringList();ntm

		int j = 0;
		for (int i = 0; i < lcm->metaList.size(); i++)
		{
			if (i % 2 == 0)
				j += 1;

			QCheckBox *metaBox = new QCheckBox();
			metaBox->setText(lcm->metaList[i]);
			gbox->addWidget(metaBox, j, i % 2);
			group->addButton(metaBox);

			if (metalist_avail.contains(lcm->metaList[i]))
			{
				metaBox->setChecked(true);
				selMetaList.push_back(metaBox->text());

			}
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
	//openMaskAct->setEnabled(enabled);
	slabMenu->setEnabled(enabled);
	roiMenu->setEnabled(enabled);
}

/***** widget menu actions *****/
void MainWindow::createActions()
{
	QMenu *fileMenu = menuBar()->addMenu(tr("File"));
	slabMenu = menuBar()->addMenu(tr("Slab"));
	roiMenu = menuBar()->addMenu(tr("Mask"));
	QMenu *helpMenu = menuBar()->addMenu(tr("Help"));


	fileMenu->addAction(tr("Open T1 Image"), this, &MainWindow::openT1);
	overlaySlabAct = fileMenu->addAction(tr("Overlay Slab"), this, &MainWindow::openSlab);
	//openMaskAct = fileMenu->addAction(tr("Overlay ROI Mask Image"), this, &MainWindow::openMask);
	fileMenu->addAction(tr("Exit"), this, &QWidget::close);

	slabMenu->addAction(tr("Create Slab Image from DICOM Files"), this, &MainWindow::makeSlabFromDicom);
	slabMenu->addAction(tr("Load LCM Info"), this, &MainWindow::openLCM);
	slabMenu->addAction(tr("Load FSLVBM Segmented Images"), this, &MainWindow::loadT1Segs);
	slabMenu->addAction(tr("QC + Create Slab Mask Image"), this, &MainWindow::makeMaskFromLCM);

	roiMenu->addAction(tr("Overlay ROI Mask Image"), this, &MainWindow::openMask);
//	roiMenu->addAction(tr("Select Voxels from ROI Mask Image"), this, &MainWindow::selectVoxFromMask);
	roiMenu->addAction(tr("Save Selected Voxels as Mask Image"), this, &MainWindow::saveVoxAsMask);

	// Some menus and actions are disabled until the T1 image is fully loaded
	setEnabledT1DepMenus(false);

	// future work: add help action
	fileMenu->addSeparator();
	slabMenu->addSeparator();


	helpMenu->addAction("About", this, &MainWindow::about);
}

void MainWindow::saveVoxAsMask()
{
	Image *voxMask = new Image();
	const size_t dimX = slab->dx();
	const size_t dimY = slab->dy();
	const size_t dimZ = slab->dz();
	voxMask->setBlankImgvol(dimX, dimY, dimZ);

	for (size_t i = 0; i < dimX; i++)
	{
		for (size_t j = 0; j < dimY; j++)
		{
			for (size_t k = 0; k < dimZ; k++)
			{
				int voxnum = slab->getImgVal(i, j, k);
				if (std::find(selectedVoxs.begin(), selectedVoxs.end(), voxnum) != selectedVoxs.end())
					voxMask->setImgVal(i, j, k, voxnum);
				else
					voxMask->setImgVal(i, j, k, 0);
			}
		}
	}
	QString filename = mask->getFilePath() + "/mask.nii.gz";
	voxMask->saveImageFile(filename, slab);
	delete voxMask;

	print("[Save] Mask image (" + filename + ")");
	printLine();
}

void MainWindow::selectVoxFromMask()
{
	int mrsivoxTotNum = mrsiVoxNumX * mrsiVoxNumY * mrsiVoxNumZ + 1;

	int voxMaskCount[mrsivoxTotNum] = {0};

	for (size_t i = 0; i < slab->dx(); i++)
	{
		for (size_t j = 0; j < slab->dy(); j++)
		{
			for (size_t k = 0; k < slab->dz(); k++)
			{
				if (slab->getImgVal(i, j, k) != 0 && mask->getImgVal(i, j, k) != 0)
				{
					voxMaskCount[(int)slab->getImgVal(i, j, k)] += 1;
				}
			}
		}
	}

	float mrsiVoxVolume = mrsiVoxSizeX * mrsiVoxSizeY * mrsiVoxSizeZ;

	selectedVoxs.clear();
	for (int i = 0; i < mrsivoxTotNum; i++)
	{
		if (voxMaskCount[i] > mrsiVoxVolume * 0.8)
			selectedVoxs.push_back(i);
	}

	for(int i = 0; i < selectedVoxs.size(); i++)
		 print(QString::number(selectedVoxs[i]));

	drawPlaneAll();

	print("[Info] Voxel selection completed");
	printLine();
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
	mask->setOverlay(true);

	lcm = new LCModelData();
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

		if(isFileExists(getPrefFileName()))	{	loadPref();						}
		if(isFileExists(getSlabFileName()))	{	loadSlabImg(getSlabFileName());	}
		if(isFileExists(getLCMFileName()))
		{
			loadLCMData();
			if (slab->isAvailable() && QDir(T1->getFilePath() + "/struc").exists())
				loadT1Segs();
		}


		setSliceNum(T1);
		setEnabledT1DepMenus(true);

		printLine();
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

void MainWindow::openMask()
{
	QFileDialog dialog(this, tr("Open File"));
	dialog.setNameFilter(tr("Nifti files (*.nii.gz *.nii *.hdr)"));

	while (dialog.exec() == QDialog::Accepted && !loadMaskImg(dialog.selectedFiles().first())) {}

	selectVoxFromMask();

	//printLine();

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

	QFileInfo f(T1->getFileName());
	QString filenames[3];
	// 0: gm, 1: wm, 2: csf
	filenames[0] = f.absolutePath() + "/struc/" + f.baseName() + "_struc_GM.nii.gz";
	filenames[1]= f.absolutePath() + "/struc/" + f.baseName() + "_struc_brain_pve_2.nii.gz";
	filenames[2] = f.absolutePath() + "/struc/" + f.baseName() + "_struc_brain_pve_0.nii.gz";

	// get segmentation information by voxel
	const size_t dimX = T1->dx();
	const size_t dimY = T1->dy();
	const size_t dimZ = T1->dz();
	Image tempimg;

	tempimg.open(filenames[0], 'r');

	for (size_t i = 0; i < dimX; i++)
	{
		for (size_t j = 0; j < dimY; j++)
		{
			for (size_t k = 0; k < dimZ; k++)
			{
				if (slab->getImgVal(i, j, k) != 0)
				{
					coord abc = n2abc(slab->getImgVal(i, j, k));
					s[abc.a][abc.b][abc.c].gm  += tempimg.getImgVal(i, j, k);
				}
			}
		}
	}
	tempimg.open(filenames[1], 'r');

	for (size_t i = 0; i < dimX; i++)
	{
		for (size_t j = 0; j < dimY; j++)
		{
			for (size_t k = 0; k < dimZ; k++)
			{
				if (slab->getImgVal(i, j, k) != 0)
				{
					coord abc = n2abc(slab->getImgVal(i, j, k));
					s[abc.a][abc.b][abc.c].wm  += tempimg.getImgVal(i, j, k);
				}
			}
		}
	}
	tempimg.open(filenames[2], 'r');

	for (size_t i = 0; i < dimX; i++)
	{
		for (size_t j = 0; j < dimY; j++)
		{
			for (size_t k = 0; k < dimZ; k++)
			{
				if (slab->getImgVal(i, j, k) != 0)
				{
					coord abc = n2abc(slab->getImgVal(i, j, k));
					s[abc.a][abc.b][abc.c].csf += tempimg.getImgVal(i, j, k);
				}
			}
		}
	}

	print("[Load] GM  (" + filenames[0] + ")");
	print("[Load] WM  (" + filenames[1] + ")");
	print("[Load] CSF (" + filenames[2] + ")");

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

				// QC: check MRSI voxels that are not included in the brain (threshold: 80%)
				if (total < (mrsiVoxVolume * 0.8))
				{
					lcm->tables[i][j][k].isAvailable = false;
				}
				else
				{
					float f_gm = s[i][j][k].gm / total;
					float f_wm = s[i][j][k].wm / total;
					float f_csf = s[i][j][k].csf / total;
					// calculate partial volume corection values
					lcm->tables[i][j][k].pvc = (43300 * f_gm + 35880 * f_wm + 55556 * f_csf) / ((1 - f_csf) * 35880);
				}
			}
		}
	}

	print("[Info] Partial Volume Correction is complete.");
	printLine();
}

void MainWindow::makeMaskFromLCM()
{
	if (selMetaList.empty())
	{
		QMessageBox::critical(this, "Error!", "Please check metabolites you want to analyze", QMessageBox::Ok);
		return;
	}

	int qcMetabolite = 0;
	QString qcSD = "20", qcFWHM = "0.2", qcSNR = "-1", qcConc = "99999";
	int qcPVC = true;

	QString qcfilename = T1->getFilePath() + "/maven_info_QC.txt";
	if(isFileExists(qcfilename))
	{
		QFile file(qcfilename);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QMessageBox::critical(this, "Error!", "LCM File Open Failed.", QMessageBox::Ok);
		}
		else
		{
			QTextStream in(&file);
			in >> qcMetabolite >> qcSD >> qcFWHM >> qcSNR >> qcConc >> qcPVC;
		}
		file.close();
	}

	// future work: if no LCM data loaded, then popup message
	QDialog dialog(this);
	QFormLayout form(&dialog);
	form.addRow(new QLabel("<center>Values for Quality Check</center>"));
/*
	QComboBox *metabolites = new QComboBox();
	metabolites->addItems(lcm->metaList);
	metabolites->setCurrentIndex(qcMetabolite);
	form.addRow("Metabolite", metabolites);
*/
	QLineEdit *sdInput = new QLineEdit(&dialog);
	sdInput->setValidator(new QIntValidator(0, 100, this));
	sdInput->setText(qcSD);
	form.addRow("SD(%)", sdInput);

	QLineEdit *fwhmInput = new QLineEdit(&dialog);
	fwhmInput->setValidator(new QDoubleValidator(0, 10, 2, this));
	fwhmInput->setText(qcFWHM);
	form.addRow("FWHM", fwhmInput);

	QLineEdit *snrInput = new QLineEdit(&dialog);
	snrInput->setValidator(new QIntValidator(0, 10, this));
	if (qcSNR != "-1")
		snrInput->setText(qcSNR);
	form.addRow("SNR (optional)", snrInput);

	QLineEdit *concInput = new QLineEdit(&dialog);
	concInput->setValidator(new QIntValidator(0, 10000, this));
	if (qcConc != "99999")
		concInput->setText(qcConc);
	form.addRow("Conc (optional)", concInput);
	//
	QCheckBox *pvcCheck = new QCheckBox(&dialog);
	pvcCheck->setChecked(qcPVC);
	form.addRow("PVC (optional)", pvcCheck);
	//
	QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
	form.addRow(&buttonBox);
	QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
	QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

	if (dialog.exec() == QDialog::Accepted)
	{
		int sd = sdInput->text().toInt();
		float fwhm = fwhmInput->text().toFloat();
		int snr = -1;
		int conc = 99999;

		if (!snrInput->text().isEmpty())
			snr = snrInput->text().toInt();

		if (!concInput->text().isEmpty())
			conc = concInput->text().toInt();

		bool pvc = pvcCheck->isChecked();

		for (int i = 0; i < selMetaList.size(); i++)
		{
			string metabolite = selMetaList[i].toStdString();
			voxelQualityCheck(metabolite, sd, fwhm, snr, conc, pvc);
			QString maskfilename = getMaskFileName(metabolite, sd, fwhm, snr, conc, pvc);
			makeMask(metabolite, pvc);
			mask->saveImageFile(maskfilename, T1);
			print("[Save] Mask image (" + maskfilename + ")");
			loadMaskImg(maskfilename);
			//loadSlabImg(maskfilename);
//			print("[Load] Mask image (" + maskfilename + ")");
//

			// print
			float avg = calAvgConc(metabolite);
			print(QString::fromStdString(metabolite) + ": " + QString::number(avg));
		}


		QFile file(qcfilename);
		QTextStream out(&file);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QMessageBox::critical(this, "Error!", "Prefernces File Creation Failed.", QMessageBox::Ok);
		}
		out << sdInput->text() << " " << fwhmInput->text() << " " << QString::number(snr) << " " << QString::number(conc) << " " << pvcCheck->isChecked();
		file.close();
	}
	printLine();
}


QString MainWindow::getMaskFileName(string metabolite, int sd, float fwhm, int snr, int conc, bool pvc)
{
	QString appendstr;
	if (snr != -1)		{	appendstr.append("_snr" + QString::number(snr));	}
	if (conc != 99999)	{	appendstr.append("_conc" + QString::number(conc));	}
	if (pvc)			{	appendstr.append("_pvc");							}

	return T1->getFileName("_mask_" + QString::fromStdString(metabolite)
							+ "_sd" + QString::number(sd)
							+ "_fwhm" + QString::number(fwhm)
							+ appendstr);
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
		sliceSpinBox[i]->setValue(1);
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
	print("[Load] DICOM directory (" + dir + ")");
	DcmFileFormat fileformat;
	OFCondition status;
	OFString seriesDescription;
	DcmSequenceOfItems *sqi;
	DcmItem *item;
	bool T1flag = false;
	bool MRSIflag = false;

	// interleaving if the number of dicom files > 100
	int count = 0;
	bool interleaving;
//	interleaving = true;
	interleaving = false;

	while (it.hasNext())
	{
		if (interleaving)
		{
			count++;
			if (count%100 != 0)
			{
				it.next();
				continue;
			}
		}

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
					// Pixel spacing, slice thickness
					/*
					DcmDataset *dset = fileformat.getDataset();
					success *= dset->findAndGetFloat64(DcmTag(0x0028, 0x0030), mrsiVoxSizeX, 0, false).good(); // Pixel Spacing 0
					success *= dset->findAndGetFloat64(DcmTag(0x0028, 0x0030), mrsiVoxSizeY, 1, false).good(); // Pixel Spacing 1
					success *= dset->findAndGetFloat64(DcmTag(0x0018, 0x0050), mrsiVoxSizeZ, 0, false).good(); // Slice Thickness
					*/
					mrsiVoxSizeX = 6.875;
					mrsiVoxSizeY = 6.875;
					mrsiVoxSizeZ = 15;

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
					loadPref();
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
	slab->setOverlay(true);
	slab->open(fileName, 'r');
	print("[Load] Slab image (" + fileName + ")");
	return true;
}


bool MainWindow::loadMaskImg(const QString &fileName)
{
	delete mask;
	mask = new Image();
	mask->setOverlay(true);
	mask->open(fileName, 'r');
	print("[Load] Mask image (" + fileName + ")");
	drawPlaneAll();
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
	lcm->setLCMInfo(dir, mrsiVoxNumX, mrsiVoxNumY, mrsiVoxNumZ);
	setLCMLayout();
//	print("[Load] LCModel table files (" + dir + ", " + QString::number(filecount) + " files)");
	print("[Load] LCModel table files (" + dir);
	return true;
}


void MainWindow::presentLCMInfo()
{
	QString info_str;
	map<string, Metabolite>::iterator metaPos;
	coord abc = n2abc(selectedVoxel);
	TableInfo temp = lcm->tables[abc.a][abc.b][abc.c];
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

	if (mask->isAvailable())
	{
		overlayimg = mask->getPlaneImage(planetype, slice);
		baseimg = overlayImage(baseimg, overlayimg);
	}

	if (slab->isAvailable())
	{
		overlayimg = slab->getPlaneImage(planetype, slice, selectedVoxs);
//		changeSelectedVoxColor(&overlayimg, planetype, qRgba(255, 0, 0, 255));
		baseimg = overlayImage(baseimg, overlayimg);
	}

	// draw part
	plane[planetype]->setPixmap(QPixmap::fromImage(baseimg.scaled(planeSize, planeSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
}

/*
void MainWindow::changeSelectedVoxColor(QImage *img, int planetype, QRgb color)
{
	int width = img->width();
	int height = img->height();

	int temp;

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			if (planetype == CORONAL)
				temp = (int)slab->getImgVal(i, sliceNum[planetype], j);
			else if (planetype == SAGITTAL)
				temp = (int)slab->getImgVal(sliceNum[planetype], i, j);
			else if (planetype == AXIAL)
				temp = (int)slab->getImgVal(i, j, sliceNum[planetype]);

			if (temp == 1360)
				img->setPixel(i, j, color);
		}
	}
}
*/

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
	T1->setIntensity(intensity);
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

				if (lcm->tables != NULL && selectedVoxel)
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
/*
void MainWindow::changeVoxelValues(float value, bool on)
{
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
}
*/

void MainWindow::voxelQualityCheck(string metabolite, int sd, float fwhm, int snr, int conc, bool pvc)
{
	if (sd == -1 || fwhm == -1)
	{
		// exception -- not available sd, fwhm values
	}
	else if (lcm->tables == NULL)
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
					if (lcm->tables[i][j][k].isAvailable)
					{
						map<string, Metabolite>::iterator tempPos;
						tempPos = lcm->tables[i][j][k].metaInfo.find(metabolite);
						if (tempPos != lcm->tables[i][j][k].metaInfo.end())
						{
							float conc_temp;
							if (pvc){	conc_temp = tempPos->second.conc * lcm->tables[i][j][k].pvc;	}
							else	{	conc_temp = tempPos->second.conc;	}

							if (tempPos->second.sd > sd || conc_temp > conc || lcm->tables[i][j][k].fwhm > fwhm || lcm->tables[i][j][k].snr < snr)
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
		print("[Info] All QC values of slab voxels are changed.");
	}
}

void MainWindow::makeMask(string metabolite, bool pvc)
{
	mask->setBlankImgvol(slab->dx(), slab->dy(), slab->dz());
	for (size_t i = 0; i < slab->dx(); i++)
	{
		for (size_t j = 0; j < slab->dy(); j++)
		{
			for (size_t k = 0; k < slab->dz(); k++)
			{
				if (slab->getImgVal(i, j, k) != 0)
				{
					coord abc = n2abc(slab->getImgVal(i, j, k));

					if (lcm->tables[abc.a][abc.b][abc.c].fwhm == -1)
						mask->setImgVal(i, j, k, 0);
					else
					{
						map<string, Metabolite>::iterator tempPos = lcm->tables[abc.a][abc.b][abc.c].metaInfo.find(metabolite);

						if (tempPos != lcm->tables[abc.a][abc.b][abc.c].metaInfo.end())
						{
							if (tempPos->second.qc && lcm->tables[abc.a][abc.b][abc.c].isAvailable)
							{
								//imagevol[i][j][k] = 1;
								if (pvc){	mask->setImgVal(i, j, k, tempPos->second.conc * lcm->tables[abc.a][abc.b][abc.c].pvc);	}
								else	{	mask->setImgVal(i, j, k, tempPos->second.conc);	}
							}
							else
								mask->setImgVal(i, j, k, 0);
						}
					}
				}
			}
		}
	}
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

	QSettings settings(T1->getFilePath()+"/maven_settings.txt", QSettings::NativeFormat);
	QString metalist="";
	for (int i = 0; i < selMetaList.size(); i++)
	{
		metalist += selMetaList[i];
		metalist += " ";
	}
	settings.setValue("metalist", metalist);
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
			print(QString::fromStdString(metabolite) + ": " + QString::number(avg));
			//string text = metabolite + ": " + std::to_string(avg) + "\n";
			//infotext.append(QString::fromStdString(text));
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
				if (lcm->tables[i][j][k].isAvailable)
				{
					map<string, Metabolite>::iterator tempPos;
					tempPos = lcm->tables[i][j][k].metaInfo.find(metabolite);

					if (tempPos != lcm->tables[i][j][k].metaInfo.end())
					{
						if (tempPos->second.qc)
						{
							count++;
							sum += tempPos->second.conc * lcm->tables[i][j][k].pvc;
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
		////
		bool pvc = true;
		////
		string metabolite = majorList[i].toStdString();
		voxelQualityCheck(metabolite, sd, fwhm, snr, conc, pvc);
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
	slabvol->saveImageFile(filename, T1);
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

				if (lcm->tables[i][j][k].isAvailable)
				{
					string str = "";
					map<string, Metabolite>::iterator metaPos;

					for (metaPos = lcm->tables[i][j][k].metaInfo.begin(); metaPos != lcm->tables[i][j][k].metaInfo.end(); metaPos++)
					{
						out << QString::fromStdString(metaPos->second.name) << "\t"
							<< metaPos->second.conc << "\t"
							<< metaPos->second.sd << "\t"
							<< metaPos->second.ratio << "\t";
					}

					out << lcm->tables[i][j][k].fwhm << "\t" << lcm->tables[i][j][k].snr << "\n";
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

	if (lcm->tables == NULL)
	{
		lcm->tables = new TableInfo **[mrsiVoxNumZ];

		for (int i = 0; i < mrsiVoxNumZ; i++)
		{
			lcm->tables[i] = new TableInfo*[mrsiVoxNumY];

			for (int j = 0; j < mrsiVoxNumY; j++)
				lcm->tables[i][j] = new TableInfo[mrsiVoxNumX];
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
			lcm->tables[a][b][c].isAvailable = false;
		else
		{
			lcm->tables[a][b][c].isAvailable = true;
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
				lcm->tables[a][b][c].metaInfo[metainfo.name] = metainfo;

				if (lcm->metaList.empty() || !lcm->metaList.contains(QString::fromStdString(metainfo.name)))   // To-do: call routine just once
					lcm->metaList.push_back(QString::fromStdString(metainfo.name));
			}

			lcm->tables[a][b][c].fwhm = tokens[tokenLen - 2].toFloat();
			lcm->tables[a][b][c].snr = tokens[tokenLen - 1].toInt();
		}
	}

//	saveLCMData();	// test for equality
	print("[Load] LCModel data (" + filename + ")");
	setLCMLayout();
}


QString MainWindow::getSlabFileName()	{	return T1->getFileName("_slab");				}
QString MainWindow::getLCMFileName()	{	return T1->getFileBaseName() + ".lcm";			}
QString MainWindow::getPrefFileName()	{	return T1->getFilePath() + "/maven_info.txt";	}

void MainWindow::print(QString str)
{
	str.replace("[", "<font color='blue'>[");
	str.replace("]", "]</font>");
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
//	print("[Save] Preferences file (" + filename + ")");
}

void MainWindow::loadPref()
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
//	print("[Load] Preferences file (" + filename + ")");
}


inline bool MainWindow::isFileExists(QString filename)
{
	struct stat buffer;
	return (stat (filename.toStdString().c_str(), &buffer) == 0);
}
