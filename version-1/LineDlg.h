
// LineDlg.h : 헤더 파일
#include"Line.h"
#include<opencv\cv.h>
#include"CamOpenCV.h"
#include"LaneDetect.h"
#pragma once
#include "afxwin.h"


// CLineDlg 대화 상자
class CLineDlg : public CDialogEx
{
// 생성입니다.
public:
	CLineDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	CCamOpenCV* _cam;
	CLaneDectect* _lane;
	 void DoLaneDetect();
// 대화 상자 데이터입니다.
	enum { IDD = IDD_LINE_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CEdit pathname;
	afx_msg void OnBnClickedButton1();
	;
	afx_msg void OnBnClickedCancel();
};
