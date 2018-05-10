
// Image_CalibrationDlg.h : header file
//

#pragma once
#include "vector"
#include "opencv2/stitching/detail/exposure_compensate.hpp"
#include "opencv2/opencv_modules.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/core/core.hpp"
#include "xfeatures2d.hpp"
#include "algorithm"

using namespace std;
using namespace cv;

// CImageCalibrationDlg dialog
class CImageCalibrationDlg : public CDialog
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

	VideoCapture capture;
	Mat			 cap_frame;
	Mat			 trs_img;
	BOOL		 cam_sgn = FALSE;
	BOOL		 shot_sgn = FALSE;

	// information
	CEdit info_;
	CComboBox camsel_;
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl focus;
	afx_msg void OnCbnSelchangecamsel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void disp_image(UINT disp_ID, Mat dsp_img, CWnd* pt);
	afx_msg void OnBnClickedshot();
	CButton shot;
	CButton calc;
	afx_msg void OnBnClickedcalc();
};
