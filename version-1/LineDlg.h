
// LineDlg.h : ��� ����
#include"Line.h"
#include<opencv\cv.h>
#include"CamOpenCV.h"
#include"LaneDetect.h"
#pragma once
#include "afxwin.h"


// CLineDlg ��ȭ ����
class CLineDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CLineDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	CCamOpenCV* _cam;
	CLaneDectect* _lane;
	 void DoLaneDetect();
// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LINE_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
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
