
// GraphProjectDlg.h : header file
//

#pragma once
#include <memory>
#include<iostream>
#include<vector>
#include "Node.h"
using namespace std;

enum Mode {DEFAULT, AddNode, AddAdj, MoveNode, RemoveNode , EditNode};

// CGraphProjectDlg dialog
class CGraphProjectDlg : public CDialogEx
{
// Construction
public:
	CGraphProjectDlg(CWnd* pParent = nullptr);	// standard constructor
	~CGraphProjectDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GRAPHPROJECT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CBrush nodeBrushRed;
	CBrush nodeBrushGreen;
	CBrush nodeBrushDefault;
	CPen linePen;
	Mode mode;
	vector<Node *> Nodes; // vector that stores the data on all drawn nodes
	vector<Edge *> Edges; // vector that stores data on all drawn edges
	Node *movedNode; // pointer that points to moving node
	Node *selectedNode; // pointer that points to selected node
	CRect win; // window rectangle
	CRect drawArea, drawAreaOffset; // window of draw area + draw area offset to avoid collison with walls
	int numOfNodes; 
	int numOfEdges;
	CComboBox nodeType; // control for shape list
	CString nodeTypeString; // string for shape list control
	CMFCColorButton nodeColorControl; // color button control
	int nodeSize; // in development
public:
	vector<Edge *>::iterator edgeContains(const Node *, const Node *);
	vector<Node *>::iterator nodeSearch(const CPoint &) ;
	void deleteEdges(const Node *);
	void Clear();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedRadioAddNodes();
	afx_msg void OnBnClickedRadioAddAdj();
	afx_msg void OnBnClickedRadioMoveNodes();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCbnSelchangeComboNodeSelection();
	afx_msg void OnBnClickedRadioRemoveNode();
	afx_msg void OnBnClickedComplementGraph();	
	afx_msg void OnBnClickedMfcnodecolorbutton();
	afx_msg void OnBnClickedRadioEditNode();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButtonClear();
};
