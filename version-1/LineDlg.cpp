// LineDlg.cpp : 구현 파일
#include"Line.h"
#include "stdafx.h"
#include "LineDlg.h"
#include "afxdialogex.h"
#include<opencv\cv.h>
#include<opencv\highgui.h>
#include"LaneDetect.h"			//이전 시간값을 유지하기위해서 LaneDect의 변수를 빌리기 위해서 헤더 선언


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLineDlg 대화 상자



CLineDlg::CLineDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLineDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	_cam = NULL;
	_lane = NULL;
}

void CLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, pathname);
}

BEGIN_MESSAGE_MAP(CLineDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CLineDlg::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CLineDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &CLineDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CLineDlg 메시지 처리기

BOOL CLineDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	_cam = new CCamOpenCV("Lane4.mp4");
	_lane = new CLaneDectect();
	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.
	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CLineDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CLineDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CLineDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLineDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK){
		if (_cam) delete _cam;
		_cam = new CCamOpenCV(dlg.GetPathName());
		pathname.SetWindowTextA(dlg.GetPathName());
		UpdateData(true);
		SetTimer(1000,30, 0);
	}
}

void CLineDlg::DoLaneDetect()
{
	assert(_cam);
	IplImage* src = _cam->CaptureImage();
	if (src){
		CvRect roi = cvRect(0, src->height * 3/6, src->width , src->height/6);
		cvSetImageROI(src, roi);

		IplImage* src_roi = cvCreateImage(cvSize(roi.width, roi.height), src->depth, src->nChannels);
		cvCopy(src, src_roi);
		cvResetImageROI(src);

		IplImage* dst = cvCreateImage(cvGetSize(src_roi), IPL_DEPTH_8U, 3);
		cvCopy(src_roi, dst, 0);
		_lane->LaneDetect(src_roi, dst);
		cvReleaseImage(&src_roi);
		cvReleaseImage(&dst);
	}
}

void CLineDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	DoLaneDetect();
	CDialogEx::OnTimer(nIDEvent);
}

void CLineDlg::OnBnClickedButton1()
{
	KillTimer(1000);
	DoLaneDetect();
}



void CLineDlg::OnBnClickedCancel(){
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}
