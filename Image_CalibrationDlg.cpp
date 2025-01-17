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
#include "conio.h"
#include "cmath"

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
	ON_CBN_DROPDOWN(IDC_camsel_, &CImageCalibrationDlg::OnCbnDropdowncamsel)
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
  
	camera_confirm();
	CenterWindow();
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
	if (sld_fun)
	{
		cur_sel = focus.GetPos();
		if (cam_sgn)
		{
			capture.set(CAP_PROP_FOCUS, cur_sel);
		}
		else if (shot_sgn)
		{
			OnBnClickedcalc();
		}
		//vector<Vec4i> hierarchy;
	}
	
	*pResult = 0;
}

//function select
void CImageCalibrationDlg::OnCbnSelchangecamsel()
{
	// TODO: Add your control notification handler code here
	index_ = camsel_.GetCurSel();
	if (index_ == camsel_.GetCount()- 6)//from files
	{
		OnBnClickedfilechoose();
		index_ = -1;
		shot_sgn = TRUE;
	}
	else if (index_ == camsel_.GetCount() - 5)
	{
		for (size_t i = 0; i < devices.size(); i++)
		{
			cameras[i].StopGrabbing();
		}
		KillTimer(0);
		capture.release();
		detect_sgn = FALSE;
		Invalidate();
		index_ = -1;
		file_nme.clear();
		destroyAllWindows();
		sld_fun = FALSE;
	}
	else if (index_ == camsel_.GetCount() - 4)//undistortion
	{
		Mat src;
		Mat distortion;
		Mat camera_matrix = Mat(3, 3, CV_32FC1);
		Mat distortion_coefficients;

		src = trs_img.clone();
		//导入相机内参和畸变系数矩阵
		FileStorage file_sto(cam_calib.outputFile, FileStorage::READ);//cam_calib.outputFile"..\\c920r-calib_1813LZ09KRA8.xml"
		file_sto["intrinsic"] >> camera_matrix;//camera_matrix
		file_sto["distortion"] >> distortion_coefficients;//distortion_coefficients
		file_sto.release();

		//矫正
		undistort(src, distortion, camera_matrix, distortion_coefficients);
		if (distortion.data)
		{
			disp_image(IDC_calibed_image, distortion, img_calib);
		}
	}
	else if (index_ == camsel_.GetCount() - 3)
	{
		//Matrix_calc();
		
		//double* corner_tmp_y = new double[4]{ 0.0 };
		//FILE*	fpin;
		//FILE*	fprt_;
		//AllocConsole();//注意检查返回值
		//freopen_s(&fprt_, "CONOUT$", "w", stdout);
		//freopen_s(&fpin, "CONIN$", "r", stdin);
		//std::cout.clear();
		//for (size_t i = 0; i < 4; i++)
		//{
		//	cout << corner_tmp_y[i] << endl;
		//}
		//system("pause");
		//std::cout.clear();
		//fclose(fprt_);
		//fclose(fpin);
		//fpin = NULL;
		//FreeConsole();
		



	}
	else if (index_ == camsel_.GetCount() - 2)//marker build
	{
		///マーカー画像を生成
		dictionary = cv::aruco::getPredefinedDictionary(DICT_6X6_50);
		const auto markerSidePixels = 160;
		const auto columns = 5;
		const auto rows = 4;
		const auto margin = 40;

		auto width = columns * markerSidePixels + margin * (columns + 1);
		auto height = rows * markerSidePixels + margin * (rows + 1);

		auto id = 0;
		cv::Rect roi(0, 0, markerSidePixels, markerSidePixels);
		cv::Mat marker(cv::Size(width, height), CV_8UC1, cv::Scalar::all(255));
		cv::Mat markerImage;

		for (auto y = 0; y < rows; y++)
		{
			roi.y = y * markerSidePixels + margin * (y + 1);
			for (auto x = 0; x < columns; x++)
			{
				roi.x = x * markerSidePixels + margin * (x + 1);
				cv::Mat roiMat(marker, roi);
				cv::aruco::drawMarker(dictionary, id++, markerSidePixels, markerImage, 1);
				markerImage.copyTo(roiMat);
			}
		}
		if (marker.data)
		{
			disp_image(IDC_ori_image, marker, img_calib);
		}
		imwrite("..//chamarkers.bmp", marker);
	}
	else if (index_ == camsel_.GetCount() - 1)//demo
	{
		shot.SetWindowText(L"Cam_Pos\r\nCalib");
		//cam_calib.capture = new VideoCapture[2];
		//for (size_t p = 0; p < 1; p++)
		//{
		//	cam_calib.capture[p].open(p);
		//	if (!cam_calib.capture[p].isOpened())
		//	{
		//		tmp_msg.Format(L"NO.%d camera not ready.", p + 1);
		//		MessageBox(tmp_msg);
		//		return;
		//	}
		//	cam_calib.capture[p].set(CAP_PROP_FRAME_WIDTH, 1280);//640 2040
		//	cam_calib.capture[p].set(CAP_PROP_FRAME_HEIGHT, 720);//360 1086
		//}
		//SetTimer(1, 50, NULL);
		cam_sgn = TRUE;
		shot_sgn = FALSE;
		Mat data_;
		data_ = Mat::ones(4, 3, CV_32FC1);
		//p1
		data_.at<float>(0, 0) = -155;
		data_.at<float>(0, 1) = 45;
		data_.at<float>(0, 2) = 0;
		//p2			       
		data_.at<float>(1, 0) = -155;
		data_.at<float>(1, 1) = -45;
		data_.at<float>(1, 2) = 0;
		//p3			       
		data_.at<float>(2, 0) = -190;
		data_.at<float>(2, 1) = -45;
		data_.at<float>(2, 2) = 0;
		//p4			       
		data_.at<float>(3, 0) = -190;
		data_.at<float>(3, 1) = 45;
		data_.at<float>(3, 2) = 0;

		////p1
		//data_.at<float>(0, 0) = 150;
		//data_.at<float>(0, 1) = -45;
		//data_.at<float>(0, 2) = -30;
		////p2			       
		//data_.at<float>(1, 0) = 150;
		//data_.at<float>(1, 1) = 35;
		//data_.at<float>(1, 2) = -30;
		////p3			       
		//data_.at<float>(2, 0) = 150;
		//data_.at<float>(2, 1) = 35;
		//data_.at<float>(2, 2) = -60;
		////p4			       
		//data_.at<float>(3, 0) = 150;
		//data_.at<float>(3, 1) = -45;
		//data_.at<float>(3, 2) = -60;
		FileStorage fs;
		fs.open("..\\coor.xml", FileStorage::WRITE);
		fs<< "coor" << data_;
		fs.release();
	}
	else
	{
		sld_fun = TRUE;
		if (devices.size() > 0)
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
				}
				converter.OutputPixelFormat = PixelType_BGR8packed;
				cameras[i].AcquisitionMode.SetValue(AcquisitionMode_Continuous);
				cameras[i].StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);
			}
		}
		else
		{
			capture.open(index_);
			capture.set(CAP_PROP_FRAME_WIDTH, 1280);//640 2040
			capture.set(CAP_PROP_FRAME_HEIGHT, 720);//360 1086
			SetTimer(0, 50, NULL);
			cam_sgn = TRUE;
			shot_sgn = FALSE;
		}
	}
	camsel_.SetCurSel(-1);
	info_.SetWindowText(L"");
}

void CImageCalibrationDlg::OnCbnDropdowncamsel()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	if (no_camsgn)
	{
		info_.SetWindowText(L"");
		camera_confirm();
	}
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
			BOOL op_sgn = detect_markers(cap_frame, search_mode);
			if (!op_sgn)
			{
				info_.SetWindowText(L"Not found.");
			}
		}
		break;
	}
	case 1:
	{
		cam_calib.capture[0] >> cap_frame;
		if (cap_frame.data)
		{
			disp_image(IDC_ori_image, cap_frame, img_calib);
		}
		cam_calib.capture[1] >> trs_img;
		if (trs_img.data)
		{
			disp_image(IDC_calibed_image, trs_img, img_calib);
		}
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

void CImageCalibrationDlg::camera_confirm()
{
	camsel_.ResetContent();
	// Get the transport layer factory.
	CTlFactory& tlFactory = CTlFactory::GetInstance();
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
				no_camsgn = FALSE;
			}
		}
		if (!number)
		{
			tmp_msg.Format(L"No USB camera present.\r\n");
			info_.ReplaceSel(tmp_msg);
			no_camsgn = TRUE;
		}
	}
	else
	{
		cameras = new Camera_t[devices.size()];
		targetImage = new CPylonImage[devices.size()];
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
			cameras[i].Width.SetValue(int64_t(2040));
			cameras[i].Height.SetValue(int64_t(1086));
			cameras[i].CenterX.SetValue(TRUE);
			cameras[i].CenterY.SetValue(TRUE);
			cameras[i].SetCameraContext(i);
		}
		no_camsgn = FALSE;
	}
	camsel_.InsertString(camsel_.GetCount(), L"From files");
	camsel_.InsertString(camsel_.GetCount(), L"Stop Grabbing");
	camsel_.InsertString(camsel_.GetCount(), L"Undistortion"); 
	camsel_.InsertString(camsel_.GetCount(), L"Matrix_calc");
	camsel_.InsertString(camsel_.GetCount(), L"Marker Build");
	camsel_.InsertString(camsel_.GetCount(), L"Position Calibration Demo");
}

//shot
void CImageCalibrationDlg::OnBnClickedshot()
{
	// TODO: Add your control notification handler code here
	BOOL op_sgn = FALSE;
	if (index_ == -1)
	{
		info_.SetWindowTextW(L"");
		if (file_nme.size() > 0)
		{
			USES_CONVERSION;
			img_nme = T2A(file_nme[file_nme.size() - 1].GetBuffer(0));
			file_nme[file_nme.size() - 1].ReleaseBuffer();
			test_img = imread(img_nme);
		}
		else
		{
			capture >> cap_frame;
			test_img = cap_frame;
		}
		op_sgn = detect_markers(test_img, search_mode);//8:6 12:7
		if (!op_sgn)
		{
			info_.ReplaceSel(L"Not Found.\r\n");
		}
		//s_sgn = TRUE; 
	}
	else if (index_ == camsel_.GetCount() - 1)//demo
	{
		KillTimer(1);
		cam_calib.manual_ = FALSE;
		Mat* capd_img = new Mat[2];
		capd_img[0] = imread("..\\tcam_ini1.bmp");
		capd_img[1] = imread("..\\tcam_ini1.bmp");
		for (size_t q = 0; q < 2; q++)
		{
		//	cam_calib.capture[q] >> capd_img[q];
			cam_calib.cam_id = q;
			op_sgn = detect_markers(capd_img[q], search_mode);//8:6 12:7
			if (!op_sgn)
			{
				info_.ReplaceSel(L"Not Found.\r\n");
			}
		}
		FileStorage fs;
		Mat		op1;
		Mat		op2;
		fs.open("..\\c0.xml", FileStorage::READ);//"..\\R_t.xml"
		fs["R_t"] >> op1;
		fs.release();
		fs.open("..\\c1.xml", FileStorage::READ);//"..\\R_t.xml"
		fs["R_t"] >> op2;
		fs.release();
		cam_calib.tmp = op2*op1.inv();
		fs.open("..\\Lm.xml", FileStorage::WRITE);
		fs << "Lm" << cam_calib.tmp;
		fs.release();
		delete[] capd_img;
		//SetTimer(1, 50, NULL);
		shot.SetWindowText(L"Shot");
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
		op_sgn = detect_markers(cap_frame, search_mode);//8:6 12:7
		if (!op_sgn)
		{
			info_.ReplaceSel(L"Not Found.\r\n");
		}
		camsel_.SetCurSel(-1);
		detect_sgn = TRUE;
	}
}

//calculate
void CImageCalibrationDlg::OnBnClickedcalc()
{
	// TODO: Add your control notification handler code here
	info_.SetWindowText(L"");
	if (file_nme.size() > 0)
	{
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
		imwrite("..\\threshold.bmp", dst);
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
	else//calib
	{
		if (!cam_calib.manual_)//demo
		{
			Mat* capd_img = new Mat[2];
			capd_img[0] = imread("..\\img_tcam_org_1.bmp");
			capd_img[1] = imread("..\\img_tcam_org_1.bmp");
			trs_img = capd_img[1].clone();
			for (size_t q = 0; q < 2; q++)
			{
			//	cam_calib.capture[q] >> capd_img[q];
				cam_calib.cam_id = q;
			}
			cam_calib.cam_id = 3;
			BOOL op_sgn = detect_markers(capd_img[0], search_mode);//8:6 12:7
			if (!op_sgn)
			{
				info_.ReplaceSel(L"Not Found.\r\n");
			}
			test_img = capd_img[1].clone();
			Matrix_calc();
			delete[] capd_img;
		}
		else
		{
			Calib_camera();
		}
	}
}

//aruco
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
	params.doCornerRefinement = TRUE;

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

BOOL CImageCalibrationDlg::detect_markers(Mat& in_image, const int mode_)
{
	if (!in_image.data)
	{
		info_.ReplaceSel(L"No valid image in.\r\n");
		return FALSE;
	}

	int														tmp_md;
	cv::Mat												inputImage;
	cv::Mat												ori_img;
	cv::Mat												cameraMatrix, distCoeffs;
	vector<int>										markerIds;
	vector<vector<Point2f>>					markerCorners, rejectedCandidates;
	aruco::DetectorParameters				parameters = aruco::DetectorParameters();
	std::vector<cv::Vec3d>						rvecs, tvecs;
	PREDEFINED_DICTIONARY_NAME		eCurrent;

	BOOL readOk = readDetectorParameters("..\\detectparam.yml", parameters);
	if (!readOk)
	{
		info_.ReplaceSel(L"Invalid detector parameters file.\r\n");
		return FALSE;
	}

	inputImage = in_image.clone();
	if (inputImage.channels() > 1)
	{
		cvtColor(inputImage.clone(), inputImage, COLOR_BGR2GRAY);
	}

	tmp_md = mode_;
	if (mode_ < 0)
	{
		eCurrent = (PREDEFINED_DICTIONARY_NAME)0;
	}
	else
	{
		eCurrent = (PREDEFINED_DICTIONARY_NAME)mode_;
	}

	while (tmp_md < aruco::DICT_ARUCO_ORIGINAL)
	{
		dictionary = cv::aruco::getPredefinedDictionary(eCurrent); // aruco::DICT_4X4_50 6x6 bits, 250 marker inside the dictionary
		aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
#pragma region DrawRejected corners
		//Mat outputImage = in_image.clone();
		//aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
		//cout << rejectedCandidates.size() << endl;
		//for (std::vector< vector<Point2f> >::iterator it = rejectedCandidates.begin(); it != rejectedCandidates.end(); ++it)
		//{
		//	vector<Point2f> sqPoints = *it;
		//	//cout << sqPoints.size() << endl;
		//	//Point pt2(it[1].x, it[1].y);
		//	line(outputImage, sqPoints[0], sqPoints[1], CV_RGB(255, 0, 0), 2);
		//	line(outputImage, sqPoints[1], sqPoints[2], CV_RGB(255, 0, 0), 2);
		//	line(outputImage, sqPoints[2], sqPoints[3], CV_RGB(255, 0, 0), 2);
		//	line(outputImage, sqPoints[3], sqPoints[0], CV_RGB(255, 0, 0), 2);
		//}
		//imwrite("..\\rejected_image.bmp", outputImage);
#pragma endregion
		if (markerIds.size() > 0)
		{
			aruco::drawDetectedMarkers(in_image, markerCorners, markerIds);
			if (!cam_calib.triger_)//calib
			{
				aruco::interpolateCornersCharuco(markerCorners, markerIds, inputImage, cam_calib.charucoboard, cam_calib.currentCharucoCorners, cam_calib.currentCharucoIds);
				if (cam_calib.currentCharucoCorners.total() > 0)
				{
					Mat imageCopy = in_image.clone();
					aruco::drawDetectedCornersCharuco(imageCopy, cam_calib.currentCharucoCorners, cam_calib.currentCharucoIds);
			//		imshow("charuco", cam_calib.currentCharucoCorners);
					tmp_msg.Format(L"Frame captured: #%d\r\n", cam_calib.frame_id++);
					info_.ReplaceSel(tmp_msg);

					cam_calib.allCorners.push_back(markerCorners);
					cam_calib.allIds.push_back(markerIds);
					cam_calib.allImgs.push_back(imageCopy);
					cam_calib.imgSize = imageCopy.size();
				}
			}
			else//detect
			{
				for (size_t h = 0; h < markerIds.size(); h++)
				{
					tmp_msg.Format(L"Found Markers Id:%d.\r\n", markerIds[h]);
					info_.ReplaceSel(tmp_msg);
				}
				tmp_msg.Format(L"Method: %d\r\n", eCurrent);
				info_.ReplaceSel(tmp_msg);
			}

			if (cam_calib.calibed)
			{
				//read parameter
				FileStorage fs(cam_calib.outputFile, FileStorage::READ);
				fs["intrinsic"] >> cameraMatrix;
				fs["distortion"] >> distCoeffs;
				fs.release();
				//
				int index_ = 0;
				for (size_t p = 0; p < markerIds.size(); p++)
				{
					if (markerIds[p] == 12)//5
					{
						index_ = p;
						break;
					}
				}
				double* corner_tmp_x = new double[4]{ 0 };
				double* corner_tmp_y = new double[4]{ 0 };
				for (size_t p = 0; p < markerCorners.size(); p++)
				{
					for (size_t q = 0; q < 4; q++)
					{
						corner_tmp_x[q] += markerCorners[p][q].x;
						corner_tmp_y[q] += markerCorners[p][q].y;
					}
				}
				for (size_t k = 0; k < 4; k++)
				{
					markerCorners[index_][k].x = corner_tmp_x[k] / markerCorners.size();
					markerCorners[index_][k].y = corner_tmp_y[k] / markerCorners.size();
				}
				cv::aruco::estimatePoseSingleMarkers(markerCorners, cam_calib.markerLength, cameraMatrix, distCoeffs, rvecs, tvecs);//mm	
#pragma region vector->matrix
				Mat rotation_matrix = Mat(3, 3, CV_32FC1, Scalar::all(0));   
				Rodrigues(rvecs[index_], rotation_matrix);
				Mat translation_matrix = Mat(tvecs[index_], true);
				cam_calib.Vw = Mat::eye(4, 4, CV_32FC1);
				rotation_matrix.copyTo(cam_calib.Vw.rowRange(0, 3).colRange(0, 3)); 
				translation_matrix.copyTo(cam_calib.Vw.rowRange(0, 3).col(3));
				cam_calib.rvecs = rotation_matrix;
				cam_calib.tvecs = translation_matrix;
				fs.open("..\\rvecs.xml", FileStorage::WRITE);
				fs << "rvecs" << cam_calib.rvecs;
				fs.release();
				fs.open("..\\tvecs.xml", FileStorage::WRITE);
				fs << "tvecs" << cam_calib.tvecs;
				fs.release();
				if (cam_calib.manual_ && !detect_sgn)
				{
					AllocConsole();//注意检查返回值
					FILE* fpin;
					FILE* fprt_;
					freopen_s(&fprt_, "CONOUT$", "w", stdout);
					string fle_name;
					freopen_s(&fpin, "CONIN$", "r", stdin);
					std::cout.clear();
					// print info
					std::cout << "rotate: " << rvecs[index_] << std::endl;
					std::cout << "trans:" << cam_calib.tvecs << std::endl;
					std::cout << "\r\ninput matrix name or type 'exit' to continue: " << std::endl;
					std::cin >> fle_name;
					if (fle_name != "exit")
					{
						fs.open(fle_name, FileStorage::WRITE);//"..\\R_t.xml"
						fs << "R_t" << cam_calib.Vw;
						fs.release();
					}
					std::cout.clear();
					fclose(fprt_);
					fclose(fpin);
					fpin = NULL;
					FreeConsole();
				}
				else
				{
					tmp_msg.Format(L"..\\c%d.xml", cam_calib.cam_id);
					USES_CONVERSION;
					char* fle_name = T2A(tmp_msg.GetBuffer(0));
					tmp_msg.ReleaseBuffer();
					fs.open(fle_name, FileStorage::WRITE);//"..\\R_t.xml"
					fs << "R_t" << cam_calib.Vw;
					fs.release();
				}
#pragma endregion
				/// draw axis for each marker
		////		vector<vector<Point2f>>	markerCorners_;
		////		vector<Point2f> temp_;
		//		for (int i = 0; i < markerIds.size(); i++)
		//		{
		//			cv::aruco::drawAxis(in_image, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 30);
		//			//for (int k = 0; k < 4; k++)
		//			//{
		//			//	//temp_.push_back(Point2f(markerCorners[i][k].x - 20, markerCorners[i][k].y + 20));
		//			//	cv::line(in_image, markerCorners[i][k], Point2f(markerCorners[i][k].x - 50, markerCorners[i][k].y + 50), cv::Scalar(0, 255, 0), 4);
		//			//	cv::line(in_image, Point2f(markerCorners[i][k].x - 50, markerCorners[i][k].y + 50), Point2f(markerCorners[i][(k + 1) % 4].x - 50, markerCorners[i][(k + 1) % 4].y + 50), cv::Scalar(0, 255, 0), 4);
		//			//}
		//		//	markerCorners_.push_back(temp_);
		//		}
				cv::aruco::drawAxis(in_image, cameraMatrix, distCoeffs, rvecs[index_], tvecs[index_], 30);//mm
			}
			//display and save image
			if (cam_calib.manual_)
			{
				disp_image(IDC_calibed_image, in_image, img_calib);
			}
			else
			{
				if (cam_calib.cam_id == 0)
				{
					disp_image(IDC_ori_image, in_image, img_calib);
				}
				else
				{
					disp_image(IDC_calibed_image, in_image, img_calib);
				}
			}
			cv::imwrite("..//drawmarkers.bmp", in_image);
			return TRUE;
		}
		if (mode_ < 0)
		{
			tmp_md++;
			eCurrent = (PREDEFINED_DICTIONARY_NAME)tmp_md;
		}
		else
		{
			break;
		}
	}
	return FALSE;
}

BOOL CImageCalibrationDlg::saveCameraParams(const string &filename, Size imageSize, float aspectRatio, int flags, const Mat &cameraMatrix, const Mat &distCoeffs, double totalAvgErr)
{
	FileStorage fs(filename, FileStorage::WRITE);

	if (!fs.isOpened())
		return false;

	SYSTEMTIME				st;
	CString						strDate;

	int								count_= cam_calib.frame_id;
	GetLocalTime(&st);
	strDate.Format(L"%4d-%2d-%2d-%2d-%2d-%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	USES_CONVERSION;
	char* calibration_time = T2A(strDate.GetBuffer(0));
	strDate.ReleaseBuffer();

	fs << "calibration_time" << calibration_time;

	fs << "image_width" << imageSize.width;

	fs << "image_height" << imageSize.height;

	fs << "frame_count" << count_;

	if (flags & CALIB_FIX_ASPECT_RATIO)
		fs << "aspectRatio" << aspectRatio;

	/*if (flags != 0)
	{
		sprintf(buf, "flags: %s%s%s%s",

			flags & CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",

			flags & CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",

			flags & CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",

			flags & CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "");
	}*/

	fs << "flags" << flags;

	fs << "intrinsic" << cameraMatrix;

	fs << "distortion" << distCoeffs;

	fs << "avg_reprojection_error" << totalAvgErr;

	return TRUE;
}

BOOL CImageCalibrationDlg::Calib_camera()
{
	// TODO: ここに実装コードを追加します.
	if (cam_calib.triger_)//capture
	{
		cam_calib.triger_ = FALSE;
		cam_calib.dictionary = cv::aruco::getPredefinedDictionary((PREDEFINED_DICTIONARY_NAME)cam_calib.dictionaryId); // aruco::DICT_4X4_50 6x6 bits, 250 marker inside the dictionary
		cam_calib.charucoboard = cv::aruco::CharucoBoard::create(cam_calib.squaresX, cam_calib.squaresY, cam_calib.squareLength, cam_calib.markerLength, cam_calib.dictionary);
		/*Mat img_board;
		cam_calib.charucoboard.draw(cv::Size(700, 500), img_board, 10, 1);
		imwrite("board.bmp", img_board);*/
		capture.open(camID);
		capture.set(CAP_PROP_FRAME_WIDTH, 1920);//640 2040
		capture.set(CAP_PROP_FRAME_HEIGHT, 1080);//360 1086
		SetTimer(0, 50, NULL);
	}
	else//judge
	{
		KillTimer(0);
		cam_calib.triger_ = TRUE;

		//calibrate
		if (cam_calib.allIds.size() < 1)
		{
			tmp_msg.Format(L"Not enough captures for calibration.\r\n");
			info_.ReplaceSel(tmp_msg);
			return 0;
		}

		// prepare data for calibration
		Mat cameraMatrix, distCoeffs;
		vector<Mat> rvecs, tvecs;
		vector<vector<Point2f>> allCornersConcatenated;
		vector<int> allIdsConcatenated;
		vector<int> markerCounterPerFrame;

		cameraMatrix = Mat::eye(3, 3, CV_64F);
		cameraMatrix.at<double>(0, 0) = 1;

		markerCounterPerFrame.reserve(cam_calib.allCorners.size());
		for (unsigned int i = 0; i < cam_calib.allCorners.size(); i++)
		{
			markerCounterPerFrame.push_back((int)cam_calib.allCorners[i].size());
			for (unsigned int j = 0; j < cam_calib.allCorners[i].size(); j++)
			{
				allCornersConcatenated.push_back(cam_calib.allCorners[i][j]);
				allIdsConcatenated.push_back(cam_calib.allIds[i][j]);
			}
		}

		// calibrate camera using aruco markers
		double arucoRepErr = aruco::calibrateCameraAruco(allCornersConcatenated, allIdsConcatenated, markerCounterPerFrame, cam_calib.charucoboard, cam_calib.imgSize, cameraMatrix, distCoeffs, noArray(), noArray(), cam_calib.calibrationFlags);

		// prepare data for charuco calibration
		int nFrames = (int)cam_calib.allCorners.size();
		vector< Mat > allCharucoCorners;
		vector< Mat > allCharucoIds;
		vector< Mat > filteredImages;
		allCharucoCorners.reserve(nFrames);
		allCharucoIds.reserve(nFrames);

		for (int i = 0; i < nFrames; i++)
		{
			// interpolate using camera parameters
			Mat currentCharucoCorners, currentCharucoIds;
			aruco::interpolateCornersCharuco(cam_calib.allCorners[i], cam_calib.allIds[i], cam_calib.allImgs[i], cam_calib.charucoboard, currentCharucoCorners, currentCharucoIds, cameraMatrix, distCoeffs);
			allCharucoCorners.push_back(currentCharucoCorners);
			allCharucoIds.push_back(currentCharucoIds);
			filteredImages.push_back(cam_calib.allImgs[i]);
		}

		if (allCharucoCorners.size() < 4)
		{
			tmp_msg.Format(L"Not enough corners for calibration.\r\n");
			info_.ReplaceSel(tmp_msg);
			return 0;
		}

		// calibrate camera using charuco
		double repError = aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, cam_calib.charucoboard, cam_calib.imgSize, cameraMatrix, distCoeffs, rvecs, tvecs, cam_calib.calibrationFlags);

		//	transpose(distCoeffs, distCoeffs.clone());
		BOOL saveOk = saveCameraParams(cam_calib.outputFile, cam_calib.imgSize, cam_calib.aspectRatio, cam_calib.calibrationFlags, cameraMatrix, distCoeffs, repError);
		if (!saveOk)
		{
			tmp_msg.Format(L"Cannot save output file.\r\n");
			info_.ReplaceSel(tmp_msg);
			return 0;
		}

		CString* out_msg = new CString[3];
		out_msg[0].Format(L"Rep Error:  %.2f\r\n", repError);
		out_msg[1].Format(L"Rep Error Aruco:  %.2f\r\n", arucoRepErr);
		out_msg[2].Format(L"Calibration saved to : %s\r\n", (CString)cam_calib.outputFile.c_str());
		for (size_t k = 0; k < 3; k++)
			info_.ReplaceSel(out_msg[k]);
		delete[] out_msg;

		if (cam_calib.showChessboardCorners)
		{
			string index_;
			// show interpolated charuco corners for debugging
			for (unsigned int frame = 0; frame < filteredImages.size(); frame++)
			{
				Mat imageCopy = filteredImages[frame].clone();
				if (cam_calib.allIds[frame].size() > 0)
				{
					if (allCharucoCorners[frame].total() > 0)
					{
						aruco::drawDetectedCornersCharuco(imageCopy, allCharucoCorners[frame], allCharucoIds[frame]);
						index_ = to_string(frame);
						imshow(index_, imageCopy);
					}
				}
			}
		}

		//clear section
		cam_calib.allCorners.clear();
		cam_calib.allIds.clear();
		cam_calib.allImgs.clear();
		cam_calib.frame_id = 0;
	}
	return true;
}

void CImageCalibrationDlg::Matrix_calc()
{
	FileStorage fs;
	Mat		op1;
	Mat		op2;
	Mat		op3;
	Mat		tmp;
	Mat		res;
	Mat		in_img;
	//Mat		
	FILE*	fpin;
	FILE*	fprt_;
	string*  fle_name = new string[3];
	///input coordinates
	Mat* cor_ = new Mat[4];
	for (size_t k = 0; k < 4; k++)
	{
		cor_[k] = Mat::ones(4, 1, CV_32FC1);
	}
	float* posi;
#pragma region Matrix_read
	if (cam_calib.manual_)
	{
		//AllocConsole();//注意检查返回值
		//freopen_s(&fprt_, "CONOUT$", "w", stdout);
		//freopen_s(&fpin, "CONIN$", "r", stdin);
		//std::cout.clear();
		//std::cout << "input first matrix name: " << std::endl;
		//std::cin >> fle_name[0];
		//std::cout << "input second matrix name: " << std::endl;
		//std::cin >> fle_name[1];
		//std::cout << "input third matrix name: " << std::endl;
		//std::cin >> fle_name[2];
		//std::cout.clear();
		//fclose(fprt_);
		//fclose(fpin);
		//fpin = NULL;
		//FreeConsole();
		fle_name[0] = "..\\c0.xml";
		fle_name[1] = "..\\c1.xml";
		fle_name[2] = "..\\c04.xml";

		fs.open(fle_name[0], FileStorage::READ);//"..\\R_t.xml"
		fs["R_t"] >> op1;
		fs.release();
		fs.open(fle_name[1], FileStorage::READ);//"..\\R_t.xml"
		fs["R_t"] >> op2;
		fs.release();
		fs.open(fle_name[2], FileStorage::READ);//"..\\R_t.xml"
		fs["R_t"] >> op3;
		fs.release();
		tmp = op1*op2.inv();
		res = tmp.inv()*op3;
		//	res = (op2*op1.inv())*op3;
		fs.open("..\\Lm.xml", FileStorage::WRITE);
		fs << "Lm" << res;
		fs.release();
		vector<vector<float> > data_(4, vector<float>(3));

		//p1
		data_[0][0] = 150;//-50;//-150;
		data_[0][1] = -15;//85;//-16;
		data_[0][2] = -45;//-15;//-20;
		//p2			       //
		data_[1][0] = 150;//-150;//-50;
		data_[1][1] = 60;//85;//-16;
		data_[1][2] = -40;//-15;//-20;
		//p3			       //
		data_[2][0] = 150;//-150;//-50;
		data_[2][1] = 60;//85;//-16;
		data_[2][2] = -67;//-50;//-60;
		//p4			       //
		data_[3][0] = 150;//-50;//-150;
		data_[3][1] = -15;//85;//-16;
		data_[3][2] = -70;//-50;//-60;

		for (size_t q = 0; q < 4; q++)
		{
			posi = cor_[q].ptr<float>(0);
			for (int r = 0; r<3; r++)
			{
				posi[r] = data_[q][r];
			}
		}

		in_img = imread("..\\scam_4.bmp");//cam2_4.png
	}
	else//demo
	{
		fs.open("..\\c3.xml", FileStorage::READ);//"..\\R_t.xml"
		fs["R_t"] >> op3;
		fs.release();
		res = cam_calib.tmp*op3;
		fs.open("..\\res.xml", FileStorage::WRITE);
		fs << "res" << res;
		fs.release();
		Mat data_;
		data_ = Mat::ones(4, 3, CV_32FC1);
		fs.open("..\\coor.xml", FileStorage::READ);//"..\\R_t.xml"
		fs["coor"] >> data_;
		fs.release();

		for (size_t q = 0; q < 4; q++)
		{
			posi = cor_[q].ptr<float>(0);
			for (int r = 0; r<3; r++)
			{
				posi[r] = data_.at<float>(q, r);
			}
		}

		in_img = test_img;
	}
#pragma endregion
	
	Mat K = Mat::zeros(3, 4, CV_32FC1);
	fs.open(cam_calib.outputFile, FileStorage::READ);
	fs["intrinsic"] >> cam_calib.cameraMatrix;
	cam_calib.cameraMatrix.copyTo(K.rowRange(0, 3).colRange(0, 3));
	Mat temp;
	Mat res_[4];
	cv::Point2f pts[4];
	for (size_t l = 0; l < 4; l++)
	{
		temp = res*cor_[l];
		res_[l] = Mat(K*temp).mul(float(1.0 / temp.at<float>(2, 0)));
		pts[l].x = res_[l].at<float>(0, 0);
		pts[l].y = res_[l].at<float>(1, 0);
	}
	///print info	
	//AllocConsole();//注意检查返回值
	//FILE* fprt;
	//freopen_s(&fprt, "CONOUT$", "w", stdout);
	//std::cout.clear();	
	//for (size_t l = 0; l < 4; l++)
	//{
	//	std::cout << "cor_[]': " << res_[l] << std::endl;
	//}
	//system("pause"); //getchar();
	//std::cout.clear();
	//fclose(fprt);
	//fprt = NULL;
	//FreeConsole();
	//display
	disp_image(IDC_ori_image, in_img, img_calib);
	for (int n = 0; n < 4; n++)
	{
		cv::line(in_img, pts[n], pts[(n + 1) % 4], Scalar(67, 125, 205), 6);//Scalar(79, 60, 58)-勝色
	}

	if (cam_calib.identify)
	{
		cv::Point2f* trs_ = new Point2f[4];
		int distance_w = std::hypot(abs(pts[0].x - pts[1].x), abs(pts[0].y - pts[1].y));
		int distance_h = std::hypot(abs(pts[0].x - pts[3].x), abs(pts[0].y - pts[3].y));

		trs_[0].x = 0;
		trs_[0].y = 0;
		//
		trs_[1].x = distance_w;
		trs_[1].y = 0;
		//
		trs_[2].x = distance_w;
		trs_[2].y = distance_h;
		//
		trs_[3].x = 0;
		trs_[3].y = distance_h;

		Mat tmp_res;
		Mat bk_img;
		Mat P = getPerspectiveTransform(pts, trs_);
		Mat src_ = trs_img;
		warpPerspective(src_, tmp_res, P, Size(distance_w, distance_h));
		imwrite("..\\PerspectiveTransform.bmp", tmp_res);
		cvtColor(tmp_res.clone(), bk_img, CV_BGR2GRAY);
		cv::Canny(bk_img.clone(), bk_img, 30, 255);
		imwrite("..\\canny.bmp", bk_img);

		cv::Scalar color;
		double iden_valve;
		int cnt_ = 0;
		bool iden_res = FALSE;
		for (int col = 0; col < bk_img.cols; col++)
		{
			if (iden_res)
			{
				break;
			}
			for (int row = 0; row < bk_img.rows; row++)
			{
				color = bk_img.at<uchar>(row, col);
				if (color[0] == 255)
				{
					cnt_++;
					if (double(double(cnt_) / (double(bk_img.cols*bk_img.rows))) > 0.135)
					{
						iden_res = TRUE;
						color = Scalar(0, 204, 51);
						break;
					}
				}
			}
		}
		iden_valve = double(double(cnt_) / (double(bk_img.cols*bk_img.rows)));
		if (!iden_res)
		{
			color = Scalar(57, 0, 190);
		}
		Mat res_ = Mat(600, 800, CV_8UC3, color);
		disp_image(IDC_ori_image, res_, img_calib);
		delete[] trs_;
	}

	disp_image(IDC_calibed_image, in_img, img_calib);
	imwrite("..\\posi-calibed.bmp", in_img);

	delete[] fle_name;
	delete[] cor_;
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
				BOOL op_sgn = detect_markers(cv_img0, search_mode);
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

//system
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
