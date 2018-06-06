
// Image_CalibrationDlg.cpp : implementation file
//
#include "stdafx.h"
#include "Image_Calibration.h"
#include "Image_CalibrationDlg.h"
#include "afxdialogex.h"
#include "iostream"
#include "CvvImage.h"
#include "vector"
#include "Strsafe.h"
#include "stdio.h"
#include "stdlib.h"
#include "iostream"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CImageCalibrationDlg* img_calib;

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CImageCalibrationDlg dialog
CImageCalibrationDlg::CImageCalibrationDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_IMAGE_CALIBRATION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageCalibrationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_info_, info_);
	DDX_Control(pDX, IDC_camsel_, camsel_);
	DDX_Control(pDX, IDC_focus, focus);
	DDX_Control(pDX, IDC_shot, shot);
	DDX_Control(pDX, IDC_calc, calc);
}

BEGIN_MESSAGE_MAP(CImageCalibrationDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_focus, &CImageCalibrationDlg::OnNMCustomdrawSlider1)
	ON_CBN_SELCHANGE(IDC_camsel_, &CImageCalibrationDlg::OnCbnSelchangecamsel)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_shot, &CImageCalibrationDlg::OnBnClickedshot)
	ON_BN_CLICKED(IDC_calc, &CImageCalibrationDlg::OnBnClickedcalc)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CImageCalibrationDlg message handlers
BOOL CImageCalibrationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	img_calib = this;

	focus.SetRange(1, 255);
	focus.SetTicFreq(1);
	focus.SetPos(5);

	camsel_.ResetContent();

	// Get the transport layer factory.
	CTlFactory& tlFactory= CTlFactory::GetInstance();

	// Get all attached devices and exit application if no device is found.
	if (tlFactory.EnumerateDevices(devices) == 0)
	{
		//throw RUNTIME_EXCEPTION("No camera present.");
		tmp_msg.Format(L"No Basler camera present.\r\n");
		info_.ReplaceSel(tmp_msg);
		int number = 0;
		while (true)
		{
			VideoCapture cap;
			cap.open(number);
			if (!cap.isOpened())
			{
				break;
			}
			else
			{
				tmp_msg.Format(L"USB camera %d\r\n", number);
				camsel_.InsertString(number, tmp_msg);
				number++;
				cap.release();
			}
		}
		if (!number)
		{
			tmp_msg.Format(L"No USB camera present.\r\n");
			info_.ReplaceSel(tmp_msg);
		}
	}
	else
	{
		cameras = new Camera_t[devices.size()];
		targetImage = new CPylonImage[devices.size()];
		int64_t wid_ = 640;
		for (size_t i = 0; i < devices.size(); ++i)
		{
			cameras[i].Attach(tlFactory.CreateDevice(devices[i]));
			cameras[i].RegisterImageEventHandler(this, RegistrationMode_Append, Ownership_ExternalOwnership);
			// Print the model name of the camera.
			CString tmp_;
			CString cam_name;
			cam_name = cameras[i].GetDeviceInfo().GetModelName();
			tmp_.Format(L"Using device: %s\r\n", cam_name);
			info_.ReplaceSel(tmp_);
			camsel_.InsertString(i, cam_name);
			cameras[i].Open();
			cameras[i].Width.SetValue(640);
			cameras[i].Height.SetValue(360);
			cameras[i].CenterX.SetValue(TRUE);
			cameras[i].CenterY.SetValue(TRUE);
			cameras[i].SetCameraContext(i);
		}
	}
	camsel_.InsertString(camsel_.GetCount(), L"From files");
	camsel_.InsertString(camsel_.GetCount(), L"Stop Grabbing");
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CImageCalibrationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CImageCalibrationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CImageCalibrationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//focus & threshold
void CImageCalibrationDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	cur_sel = focus.GetPos();
	if (cam_sgn)
	{
		capture.set(CAP_PROP_FOCUS, cur_sel);
	}
	else if (shot_sgn)
	{
		OnBnClickedcalc();
	}
	vector<Vec4i> hierarchy;

	*pResult = 0;
}

//camera sel
void CImageCalibrationDlg::OnCbnSelchangecamsel()
{
	// TODO: Add your control notification handler code here
	index_ = camsel_.GetCurSel();
	if (index_ == camsel_.GetCount()- 2)//from files
	{
		OnBnClickedfilechoose();
		index_ = -1;
		shot_sgn = TRUE;
	}
	else if (index_ == camsel_.GetCount() - 1)
	{
		for (size_t i = 0; i < devices.size(); i++)
		{
			cameras[i].StopGrabbing();
		}
		KillTimer(0);
		detect_sgn = FALSE;
		Invalidate();
	}
	else
	{
		for (size_t i = 0; i < devices.size(); i++)
		{
			AutoGainContinuous(cameras[i]);
			// Carry out luminance control by using the "continuous" exposure auto function.
			AutoExposureContinuous(cameras[i]);
			if (IsColorCamera(cameras[i]))
			{
				// Carry out white balance using the balance white auto function.
				AutoWhiteBalance(cameras[i]);
				converter.OutputPixelFormat = PixelType_BGR8packed;
			}
			cameras[i].AcquisitionMode.SetValue(AcquisitionMode_Continuous);
			cameras[i].StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);
		}
		capture.open(index_);
		capture.set(CAP_PROP_FRAME_WIDTH, 640);//640 2040
		capture.set(CAP_PROP_FRAME_HEIGHT, 360);//360 1086
		SetTimer(0, 50, NULL);
		cam_sgn = TRUE;
		shot_sgn = FALSE;
	}
	camsel_.SetCurSel(-1);
	info_.SetWindowText(L"");
}

void CImageCalibrationDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case 0:
	{
		capture >> cap_frame;
		if (cap_frame.data)
		{
			disp_image(IDC_ori_image, cap_frame, img_calib);
		}
		if (detect_sgn)
		{
			BOOL op_sgn = detect_markers(cap_frame);
			if (!op_sgn)
			{
				info_.SetWindowText(L"Not found.");
			}
		}
		break;
	}
	case 1:
	{
		
		break;
	}
	default:
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

/*Auto Function*/

void CImageCalibrationDlg::AutoWhiteBalance(Camera_t& camera)
{
	// Check whether the Balance White Auto feature is available.
	if (!IsWritable(camera.BalanceWhiteAuto))
	{
		cout << "The camera does not support Balance White Auto." << endl << endl;
		return;
	}

	// Maximize the grabbed area of interest (Image AOI).
	if (IsWritable(camera.OffsetX))
	{
		camera.OffsetX.SetValue(camera.OffsetX.GetMin());
	}
	if (IsWritable(camera.OffsetY))
	{
		camera.OffsetY.SetValue(camera.OffsetY.GetMin());
	}
	camera.Width.SetValue(camera.Width.GetMax());
	camera.Height.SetValue(camera.Height.GetMax());

	if (IsAvailable(camera.AutoFunctionROISelector))
	{
		// Set the Auto Function ROI for white balance.
		// We want to use ROI2
		camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);
		camera.AutoFunctionROIUseWhiteBalance.SetValue(false);   // ROI 1 is not used for white balance
		camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
		camera.AutoFunctionROIUseWhiteBalance.SetValue(true);   // ROI 2 is used for white balance

																// Set the Auto Function AOI for white balance statistics.
																// Currently, AutoFunctionROISelector_ROI2 is predefined to gather
																// white balance statistics.
		camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
		camera.AutoFunctionROIOffsetX.SetValue(0);
		camera.AutoFunctionROIOffsetY.SetValue(0);
		camera.AutoFunctionROIWidth.SetValue(camera.Width.GetMax());
		camera.AutoFunctionROIHeight.SetValue(camera.Height.GetMax());
	}

	cout << "Trying 'BalanceWhiteAuto = Once'." << endl;
	cout << "Initial balance ratio: ";
	camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Red);
	cout << "R = " << camera.BalanceRatio.GetValue() << "   ";
	camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Green);
	cout << "G = " << camera.BalanceRatio.GetValue() << "   ";
	camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Blue);
	cout << "B = " << camera.BalanceRatio.GetValue() << endl;

	camera.BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Once);

	// When the "once" mode of operation is selected,
	// the parameter values are automatically adjusted until the related image property
	// reaches the target value. After the automatic parameter value adjustment is complete, the auto
	// function will automatically be set to "off" and the new parameter value will be applied to the
	// subsequently grabbed images.
}

bool CImageCalibrationDlg::IsColorCamera(Camera_t& camera)
{
	GenApi::NodeList_t Entries;
	camera.PixelFormat.GetEntries(Entries);
	bool Result = false;

	for (size_t i = 0; i < Entries.size(); i++)
	{
		GenApi::INode *pNode = Entries[i];
		if (IsAvailable(pNode->GetAccessMode()))
		{
			GenApi::IEnumEntry *pEnum = dynamic_cast<GenApi::IEnumEntry *>(pNode);
			const GenICam::gcstring sym(pEnum->GetSymbolic());
			if (sym.find(GenICam::gcstring("Bayer")) != GenICam::gcstring::_npos())
			{
				Result = true;
				break;
			}
		}
	}
	return Result;
}

void CImageCalibrationDlg::AutoExposureContinuous(Camera_t& camera)
{
	// Check whether the Exposure Auto feature is available.
	if (!IsWritable(camera.ExposureAuto))
	{
		cout << "The camera does not support Exposure Auto." << endl << endl;
		return;
	}

	// Maximize the grabbed area of interest (Image AOI).
	if (IsWritable(camera.OffsetX))
	{
		camera.OffsetX.SetValue(camera.OffsetX.GetMin());
	}
	if (IsWritable(camera.OffsetY))
	{
		camera.OffsetY.SetValue(camera.OffsetY.GetMin());
	}
	camera.Width.SetValue(camera.Width.GetMax());
	camera.Height.SetValue(camera.Height.GetMax());

	if (IsAvailable(camera.AutoFunctionROISelector))
	{
		// Set the Auto Function ROI for luminance statistics.
		// We want to use ROI1 for gathering the statistics.
		if (IsWritable(camera.AutoFunctionROIUseBrightness))
		{
			camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);
			camera.AutoFunctionROIUseBrightness.SetValue(true);   // ROI 1 is used for brightness control
			camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
			camera.AutoFunctionROIUseBrightness.SetValue(false);   // ROI 2 is not used for brightness control
		}

		// Set the ROI (in this example the complete sensor is used)
		camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);  // configure ROI 1
		camera.AutoFunctionROIOffsetX.SetValue(0);
		camera.AutoFunctionROIOffsetY.SetValue(0);
		camera.AutoFunctionROIWidth.SetValue(camera.Width.GetMax());
		camera.AutoFunctionROIHeight.SetValue(camera.Height.GetMax());
	}

	// Set the target value for luminance control.
	// A value of 0.3 means that the target brightness is 30 % of the maximum brightness of the raw pixel value read out from the sensor.
	// A value of 0.4 means 40 % and so forth.
	camera.AutoTargetBrightness.SetValue(0.3);

	cout << "ExposureAuto 'GainAuto = Continuous'." << endl;
	cout << "Initial exposure time = ";
	cout << camera.ExposureTime.GetValue() << " us" << endl;

	camera.ExposureAuto.SetValue(ExposureAuto_Continuous);

}

void CImageCalibrationDlg::AutoGainContinuous(Camera_t& camera)
{
	// Check whether the Gain Auto feature is available.
	if (!IsWritable(camera.GainAuto))
	{
		cout << "The camera does not support Gain Auto." << endl << endl;
		return;
	}
	// Maximize the grabbed image area of interest (Image AOI).
	if (IsWritable(camera.OffsetX))
	{
		camera.OffsetX.SetValue(camera.OffsetX.GetMin());
	}
	if (IsWritable(camera.OffsetY))
	{
		camera.OffsetY.SetValue(camera.OffsetY.GetMin());
	}
	camera.Width.SetValue(camera.Width.GetMax());
	camera.Height.SetValue(camera.Height.GetMax());

	if (IsAvailable(camera.AutoFunctionROISelector))
	{
		// Set the Auto Function ROI for luminance statistics.
		// We want to use ROI1 for gathering the statistics.
		if (IsWritable(camera.AutoFunctionROIUseBrightness))
		{
			camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);
			camera.AutoFunctionROIUseBrightness.SetValue(true);   // ROI 1 is used for brightness control
			camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
			camera.AutoFunctionROIUseBrightness.SetValue(false);   // ROI 2 is not used for brightness control
		}

		// Set the ROI (in this example the complete sensor is used)
		camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);  // configure ROI 1
		camera.AutoFunctionROIOffsetX.SetValue(0);
		camera.AutoFunctionROIOffsetY.SetValue(0);
		camera.AutoFunctionROIWidth.SetValue(camera.Width.GetMax());
		camera.AutoFunctionROIHeight.SetValue(camera.Height.GetMax());
	}

	// Set the target value for luminance control.
	// A value of 0.3 means that the target brightness is 30 % of the maximum brightness of the raw pixel value read out from the sensor.
	// A value of 0.4 means 40 % and so forth.
	camera.AutoTargetBrightness.SetValue(0.3);

	// We are trying GainAuto = Continuous.
	cout << "Trying 'GainAuto = Continuous'." << endl;
	cout << "Initial Gain = " << camera.Gain.GetValue() << endl;

	camera.GainAuto.SetValue(GainAuto_Continuous);
}

/*Auto Function*/

void CImageCalibrationDlg::disp_image(UINT disp_ID, Mat dsp_img, CWnd* pt)
{
	IplImage cpy;
	CvvImage cimg;
	CDC* _pDC;
	HDC hDC;
	CRect rect;
	_pDC = pt->GetDlgItem(disp_ID)->GetDC();		// 获得显示控件的 DC
	hDC = _pDC->GetSafeHdc();				// 获取 HDC(设备句柄) 来进行绘图操作
	pt->GetDlgItem(disp_ID)->GetClientRect(&rect);	// 获取控件尺寸位置
	cpy = dsp_img;
	cimg.CopyOf(&cpy);						// 复制图片
	cimg.DrawToHDC(hDC, &rect);				// 将图片绘制到显示控件的指定区域内
	pt->ReleaseDC(_pDC);
}

//shot
void CImageCalibrationDlg::OnBnClickedshot()
{
	// TODO: Add your control notification handler code here
	if (index_ == -1)
	{
		info_.SetWindowTextW(L"");
		/*USES_CONVERSION;
		img_nme = T2A(file_nme[file_nme.size() - 1].GetBuffer(0));
		file_nme[file_nme.size() - 1].ReleaseBuffer();

		test_img = imread(img_nme);
		detect_markers(test_img);*/
	}
	else
	{
		capture >> cap_frame;
		if (cap_frame.data)
		{
			imwrite("..//temp.bmp", cap_frame);
			trs_img = cap_frame.clone();
			/*cam_sgn = FALSE;
			shot_sgn = TRUE;*/
		}
		camsel_.SetCurSel(-1);
		//KillTimer(0);
	}
	detect_sgn = TRUE;
}

//calculate
void CImageCalibrationDlg::OnBnClickedcalc()
{
	// TODO: Add your control notification handler code here
	info_.SetWindowText(L"");
	int cnt_ = 0;
	Mat src;
	Mat dst;
	Mat tmp_;
	CString tmp_msg;
	Point2f vec_p[4];
	RotatedRect rot_rect;
	vector<vector<Point>> contour;
	tmp_ = trs_img;
	src = trs_img.clone();
	if (src.channels() > 1)
	{
		cvtColor(src.clone(), src, COLOR_BGR2GRAY);
	}
	threshold(src, dst, cur_sel, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);//_INV | cv::THRESH_OTSU
//	adaptiveThreshold(dst, tmp_, 255, ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 13, 3);
	imwrite("..//threshold.bmp", dst);
	disp_image(IDC_calibed_image, dst, img_calib);
	//find contours
	findContours(dst, contour, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	for (int h = 0; h < contour.size(); h++)
	{
		if (contourArea(contour[h]) > 500)//5500
		{
			rot_rect = minAreaRect(contour[h]);
			rot_rect.points(vec_p);
			//-- Draw lines between the corners (the mapped object in the scene)
			for (int ind_ = 0; ind_ < 4; ind_++)
			{
				line(tmp_, vec_p[ind_], vec_p[(ind_ + 1) % 4], Scalar(0, 0, 255), 10, 8);
			}
			tmp_msg.Format(L"%d", cnt_);
			USES_CONVERSION;
			char* text_ = T2A(tmp_msg.GetBuffer(0));
			tmp_msg.ReleaseBuffer();
			double myContourAngle = rot_rect.angle;
			if (rot_rect.size.width < rot_rect.size.height)
			{
				myContourAngle = myContourAngle - 90;
			}
			putText(tmp_, text_, vec_p[0], FONT_HERSHEY_PLAIN, 3, Scalar(255, 0, 0));
			tmp_msg.Format(L"angle%d: %.2f, width%d: %.2f\r\n", cnt_, myContourAngle, cnt_, rot_rect.size.width);//rot_rect.angle
			info_.ReplaceSel(tmp_msg);
			drawContours(tmp_, contour, h, Scalar(0, 255, 0), 4, 8);
			cnt_++;
		}
	}
	tmp_msg.Format(L"Found Object: %d\r\n", cnt_);
	info_.ReplaceSel(tmp_msg);
	cnt_ = 0;
	disp_image(IDC_ori_image, tmp_, img_calib);
	imwrite("..//res.bmp", tmp_);
}

BOOL CImageCalibrationDlg::readDetectorParameters(string filename, aruco::DetectorParameters &params)
{

	FileStorage fs(filename, FileStorage::READ);

	if (!fs.isOpened())

		return false;

	fs["adaptiveThreshWinSizeMin"] >> params.adaptiveThreshWinSizeMin;

	fs["adaptiveThreshWinSizeMax"] >> params.adaptiveThreshWinSizeMax;

	fs["adaptiveThreshWinSizeStep"] >> params.adaptiveThreshWinSizeStep;

	fs["adaptiveThreshConstant"] >> params.adaptiveThreshConstant;

	fs["minMarkerPerimeterRate"] >> params.minMarkerPerimeterRate;

	fs["maxMarkerPerimeterRate"] >> params.maxMarkerPerimeterRate;

	fs["polygonalApproxAccuracyRate"] >> params.polygonalApproxAccuracyRate;

	fs["minCornerDistanceRate"] >> params.minCornerDistanceRate;

	fs["minDistanceToBorder"] >> params.minDistanceToBorder;

	fs["minMarkerDistanceRate"] >> params.minMarkerDistanceRate;

//	fs["cornerRefinementMethod"] >> params->cornerRefinementMethod;

	fs["cornerRefinementWinSize"] >> params.cornerRefinementWinSize;

	fs["cornerRefinementMaxIterations"] >> params.cornerRefinementMaxIterations;

	fs["cornerRefinementMinAccuracy"] >> params.cornerRefinementMinAccuracy;

	fs["markerBorderBits"] >> params.markerBorderBits;

	fs["perspectiveRemovePixelPerCell"] >> params.perspectiveRemovePixelPerCell;

	fs["perspectiveRemoveIgnoredMarginPerCell"] >> params.perspectiveRemoveIgnoredMarginPerCell;

	fs["maxErroneousBitsInBorderRate"] >> params.maxErroneousBitsInBorderRate;

	fs["minOtsuStdDev"] >> params.minOtsuStdDev;

	fs["errorCorrectionRate"] >> params.errorCorrectionRate;

	return true;

}

BOOL CImageCalibrationDlg::detect_markers(Mat& in_image, int mode_)
{
	if (!in_image.data)
	{
		info_.ReplaceSel(L"No valid image in.\r\n");
		return FALSE;
	}

	cv::Mat				inputImage;
	cv::Mat				ori_img;
	cv::Mat cameraMatrix, distCoeffs;
	vector<int>		markerIds;
	vector<vector<Point2f>> markerCorners, rejectedCandidates;
	aruco::DetectorParameters parameters = aruco::DetectorParameters();
	std::vector<cv::Vec3d> rvecs, tvecs;
	PREDEFINED_DICTIONARY_NAME eCurrent;

	BOOL readOk = readDetectorParameters("..\\detectparam.yml", parameters);
	if (!readOk)
	{
		info_.ReplaceSel(L"Invalid detector parameters file.\r\n");
		return FALSE;
	}

	FileStorage fs("..//c1000_calib.xml", FileStorage::READ);
	fs["intrinsic"] >> cameraMatrix;
	fs["distortion"] >> distCoeffs;
	fs.release();

	/*cv::Mat markerImage;
	cv::aruco::drawMarker(dictionary, 10, 200, markerImage, 1);
	imwrite("..//markers.bmp", markerImage);
	imshow("marker", markerImage);*/

	/*for (int i = 0; i < aruco::DICT_ARUCO_ORIGINAL; i++)
	{
		PREDEFINED_DICTIONARY_NAME eCurrent = (PREDEFINED_DICTIONARY_NAME)i;
		inputImage = in_image.clone();
		cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(eCurrent); // aruco::DICT_4X4_50 6x6 bits, 250 marker inside the dictionary

		aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
		aruco::drawDetectedMarkers(in_image, markerCorners, markerIds);
		if (markerIds.size() < 4)
		{
			//info_.ReplaceSel(L"No Markers detected.\r\n");
		}
		else
		{
			for (size_t h = 0; h < markerIds.size(); h++)
			{
				tmp_msg.Format(L"Found Markers Id:%d.\r\n", markerIds[h]);
				info_.ReplaceSel(tmp_msg);
			}
			tmp_msg.Format(L"Method: %d\r\n", eCurrent);
			info_.ReplaceSel(tmp_msg);
			cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);
			// draw axis for each marker
			for (int i = 0; i<markerIds.size(); i++)
				cv::aruco::drawAxis(in_image, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.025);

			fs.open("..//rvecs.xml", FileStorage::WRITE);
			fs << "rvecs" << rvecs;
			fs.release();

			fs.open("..//tvecs.xml", FileStorage::WRITE);
			fs << "tvecs" << tvecs;
			fs.release();

			disp_image(IDC_calibed_image, in_image, img_calib);
			cv::imwrite("..//drawmarkers.png", in_image);
			return TRUE;
		}
	}
	*/

	eCurrent = (PREDEFINED_DICTIONARY_NAME)mode_;
	inputImage = in_image.clone();
	cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(eCurrent); // aruco::DICT_4X4_50 6x6 bits, 250 marker inside the dictionary

	aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
	aruco::drawDetectedMarkers(in_image, markerCorners, markerIds);
	if (markerIds.size() < 4)
	{
		//info_.ReplaceSel(L"No Markers detected.\r\n");
		return FALSE;
	}
	else
	{
		for (size_t h = 0; h < markerIds.size(); h++)
		{
			tmp_msg.Format(L"Found Markers Id:%d.\r\n", markerIds[h]);
			info_.ReplaceSel(tmp_msg);
		}
		tmp_msg.Format(L"Method: %d\r\n", eCurrent);
		info_.ReplaceSel(tmp_msg);
		cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);
		// draw axis for each marker
		for (int i = 0; i < markerIds.size(); i++)
			cv::aruco::drawAxis(in_image, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.025);

		fs.open("..//rvecs.xml", FileStorage::WRITE);
		fs << "rvecs" << rvecs;
		fs.release();

		fs.open("..//tvecs.xml", FileStorage::WRITE);
		fs << "tvecs" << tvecs;
		fs.release();

		disp_image(IDC_calibed_image, in_image, img_calib);
		cv::imwrite("..//drawmarkers.png", in_image);
		return TRUE;
	}
}

//file choose
void CImageCalibrationDlg::OnBnClickedfilechoose()
{
	// TODO: Add your control notification handler code here
	CString szFileName;
	CString szCount;
	POSITION pos;
	int nCount = 0; //文件数量  
					//输入文件  
	CFileDialog InFile(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT, _T("bmp|*.bmp|all files(*.*)|*.*||"));
	InFile.m_ofn.nMaxFile = 10000;//文件上限  
	InFile.m_ofn.lpstrFile = (LPWSTR)malloc(InFile.m_ofn.nMaxFile * sizeof(TCHAR));
	memset(InFile.m_ofn.lpstrFile, 0, InFile.m_ofn.nMaxFile);
	//按了OK按钮，开始获取文件列表  
	if (InFile.DoModal() == IDOK)
	{
		pos = InFile.GetStartPosition();
		while (pos)
		{
			szFileName = InFile.GetNextPathName(pos);
			//这里szFileName就是文件路径  
			//做点什么事情，比如添加到列表框  
			file_nme.push_back(szFileName);

			USES_CONVERSION;
			img_nme = T2A(szFileName.GetBuffer(0));
			szFileName.ReleaseBuffer();

			test_img = imread(img_nme);
			trs_img = test_img.clone();
			if (test_img.data)
			{
				disp_image(IDC_ori_image, test_img, img_calib);
			}
			else
			{
				info_.ReplaceSel(L"Warning, input image not Valid.\r\n");
			}
			file_nme.shrink_to_fit();
			//info_.SetWindowText(L"");
		}
	}
	//释放空间  
	free(InFile.m_ofn.lpstrFile);
}

void CImageCalibrationDlg::OnImageGrabbed(CInstantCamera& cameras, const CGrabResultPtr& ptrGrabResult_act)
{
	// Image grabbed successfully?
	if (ptrGrabResult_act->GrabSucceeded())
	{
		intptr_t k = cameras.GetCameraContext();
		if (k %2 == 0)
		{
			converter.Convert(targetImage[0], ptrGrabResult_act);
			Mat cv_img0 = Mat(targetImage[0].GetHeight(), targetImage[0].GetWidth(), CV_8UC3);
			cv_img0 = cv::Mat(targetImage[0].GetHeight(), targetImage[0].GetWidth(), CV_8UC3, (uint8_t*)targetImage[0].GetBuffer());//
			disp_image(IDC_ori_image, cv_img0, img_calib);
			if (detect_sgn)
			{
				BOOL op_sgn = detect_markers(cv_img0);
				if (!op_sgn)
				{
					info_.SetWindowText(L"Not found.");
				}
			}
		}
		else
		{
			converter.Convert(targetImage[1], ptrGrabResult_act);
			Mat cv_img0 = Mat(targetImage[1].GetHeight(), targetImage[1].GetWidth(), CV_8UC3);
			cv_img0 = cv::Mat(targetImage[1].GetHeight(), targetImage[1].GetWidth(), CV_8UC3, (uint8_t*)targetImage[1].GetBuffer());//
		//	disp_image(IDC_calibed_image, cv_img0, img_calib);
		}
	}
}


void CImageCalibrationDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	delete[] cameras;
	delete[] targetImage;
	CDialog::OnClose();
}

void CImageCalibrationDlg::OnDestroy()
{
	CDialog::OnDestroy();
	// TODO: Add your message handler code here
}
