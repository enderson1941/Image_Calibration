
// Image_CalibrationDlg.h : header file
//

#pragma once
#include "vector"
#include "opencv2/opencv_modules.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/core/core.hpp"
#include "aruco.hpp"
#include "opencv2/aruco/charuco.hpp"
#include "xfeatures2d.hpp"
#include "algorithm"

#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

#include "pylon/usb/BaslerUsbCamera.h"
#include "pylon/usb/BaslerUsbInstantCamera.h"
#include "pylon/ImageEventHandler.h"
#include "ImageEventPrinter.h"
#include "pylon/ConfigurationEventHandler.h"

using namespace std;
using namespace cv;
using namespace aruco;
using namespace Pylon;
using namespace Basler_UsbCameraParams;

static const size_t c_maxCamerasToUse = 2;

// CImageCalibrationDlg dialog
class CImageCalibrationDlg : public CDialog, public CImageEventHandler
{
// Construction
public:
	CImageCalibrationDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMAGE_CALIBRATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void OnImageGrabbed(CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult);
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	typedef Pylon::CBaslerUsbInstantCamera Camera_t;

	typedef struct camera_calib
	{
		int squaresX = 7;
		int squaresY = 5;
		int calibrationFlags = 0;
		int frame_id = 0;
		int dictionaryId = 8;
		int cam_id = 0;
		float aspectRatio = 1;
		float squareLength = 50;
		float markerLength = 40;
		bool showChessboardCorners = false;
		bool refindStrategy = false;
		bool calibed = true;
		bool identify = true;

		// collect data from each frame
		VideoCapture	*	   capture;
		aruco::Dictionary dictionary;
		aruco::DetectorParameters detectorParams;
		aruco::CharucoBoard charucoboard;
		vector<vector<vector<Point2f>>> allCorners;
		vector<vector<int>> allIds;
		vector<Mat> allImgs;
		Size imgSize;
		Mat currentCharucoCorners, currentCharucoIds;
		Mat cameraMatrix;

		//estimate
		Mat rvecs, tvecs;
		Mat Hw, Vw;
		Mat tmp;

		// system
		BOOL triger_ = TRUE;
		BOOL manual_ = TRUE;
		string outputFile = "..\\c920r-calib_1813LZ09KRA8.xml";// camera_para.xml parser.get<string>(0); c920r-calib_1813LZ09KRA8
	}; 
	camera_calib cam_calib;

	int									 cur_sel = 100;
	int									 index_ = -1;
	int									 search_mode = 8;
	int									 camID = 0;
	char*							 img_nme;

	Mat								 cap_frame;
	Mat								 trs_img;
	Mat								 test_img;
	BOOL							 cam_sgn = FALSE;
	BOOL							 shot_sgn = FALSE;
	BOOL							 detect_sgn = FALSE;
	BOOL							 s_sgn = FALSE;
	BOOL							 no_camsgn = FALSE;
	BOOL							 sld_fun = FALSE;
	vector<CString>			 file_nme;
	CString							 tmp_msg;
	CEdit								 info_;
	CButton							 shot;
	CButton							 calc;
	CComboBox					 camsel_;
	CSliderCtrl						 focus;
	VideoCapture					 capture;
	
	cv::aruco::Dictionary		 dictionary;

	//pylon
	DeviceInfoList_t				 devices;
	Camera_t*					 cameras;
	// Create a target image
	CPylonImage*				 targetImage;
	// First the image format converter class must be created.
	CImageFormatConverter converter;
	// This smart pointer will receive the grab result data.
	CGrabResultPtr				 ptrGrabResult;

	void AutoWhiteBalance(Camera_t& camera);
	bool IsColorCamera(Camera_t& camera);
	void AutoExposureContinuous(Camera_t& camera);
	void AutoGainContinuous(Camera_t& camera);
	void disp_image(UINT disp_ID, Mat dsp_img, CWnd* pt);
	void camera_confirm();
	//file choose
	void OnBnClickedfilechoose();
	void Matrix_calc();

	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnDropdowncamsel();
	afx_msg void OnCbnSelchangecamsel();
	afx_msg void OnBnClickedshot();
	afx_msg void OnBnClickedcalc();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	
	//aruco
	BOOL readDetectorParameters(string filename, aruco::DetectorParameters &params);
	BOOL detect_markers(Mat& in_image, const int mode_ = 0);
	BOOL saveCameraParams(const string &filename, Size imageSize, float aspectRatio, int flags, const Mat &cameraMatrix, const Mat &distCoeffs, double totalAvgErr);
	BOOL Calib_camera();

};
