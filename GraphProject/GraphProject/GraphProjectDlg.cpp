
// GraphProjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GraphProject.h"
#include "GraphProjectDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGraphProjectDlg dialog

CGraphProjectDlg::CGraphProjectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GRAPHPROJECT_DIALOG, pParent)
	, numOfEdges(0)
	, numOfNodes(0)
	, nodeTypeString(_T(""))
	, nodeSize(0)
	
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	mode = DEFAULT;
	selectedNode = NULL;
}

CGraphProjectDlg::~CGraphProjectDlg() {
	Clear();
}

void CGraphProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_EDIT_DRAWSPACE, drawSpace);
	DDX_Text(pDX, IDC_EDIT_EDGES, numOfEdges);
	DDV_MinMaxInt(pDX, numOfEdges, 0, INT_MAX);
	DDX_Text(pDX, IDC_EDIT_NODES, numOfNodes);
	DDV_MinMaxInt(pDX, numOfNodes, 0, INT_MAX);
	DDX_Control(pDX, IDC_COMBO_NODE_SELECTION, nodeType);
	DDX_CBString(pDX, IDC_COMBO_NODE_SELECTION, nodeTypeString);
	DDX_Control(pDX, IDC_MFCNodeCOLORBUTTON, nodeColorControl);
	DDX_Slider(pDX, IDC_NODE_SLIDER, nodeSize);
	DDV_MinMaxInt(pDX, nodeSize, 10, 40);
}

BEGIN_MESSAGE_MAP(CGraphProjectDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_ADD_EDGE, &CGraphProjectDlg::OnBnClickedRadioAddAdj)
	ON_BN_CLICKED(IDC_RADIO_ADD_NODES, &CGraphProjectDlg::OnBnClickedRadioAddNodes)
	ON_BN_CLICKED(IDC_RADIO_MOVE_NODES, &CGraphProjectDlg::OnBnClickedRadioMoveNodes)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_RADIO_REMOVE_NODE, &CGraphProjectDlg::OnBnClickedRadioRemoveNode)
	ON_BN_CLICKED(IDC_COMPLEMENT_GRAPH, &CGraphProjectDlg::OnBnClickedComplementGraph)
	ON_CBN_SELCHANGE(IDC_COMBO_NODE_SELECTION, &CGraphProjectDlg::OnCbnSelchangeComboNodeSelection)
	ON_BN_CLICKED(IDC_MFCNodeCOLORBUTTON, &CGraphProjectDlg::OnBnClickedMfcnodecolorbutton)
	ON_BN_CLICKED(IDC_RADIO_EDIT_NODE, &CGraphProjectDlg::OnBnClickedRadioEditNode)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CGraphProjectDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CGraphProjectDlg::OnBnClickedButtonLoad)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CGraphProjectDlg::OnBnClickedButtonClear)
END_MESSAGE_MAP()


// CGraphProjectDlg message handlers

BOOL CGraphProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// initialize shape list combo box
	nodeType.InsertString(0, _T("Circle")); // nodeTye.AddString(_T()):
	nodeType.InsertString(1, _T("Rectangle"));
	nodeType.InsertString(2, _T("Triangle"));
	nodeType.SetCurSel(0);
	nodeType.GetLBText(nodeType.GetCurSel(), nodeTypeString);
	UpdateData(FALSE);


	// initialize draw area + offset area
	GetClientRect(&win);
	drawArea.SetRect(win.Width()/6, 0, win.Width() - win.Width()/7, win.Height());
	drawAreaOffset.SetRect(drawArea.left + SHAPE_AREA, drawArea.top + SHAPE_AREA,
		drawArea.right - SHAPE_AREA, drawArea.bottom - SHAPE_AREA);

	// initialize brush + pen for node and line drawing
	nodeBrushRed.CreateSolidBrush(RGB(255, 204, 204));
	nodeBrushGreen.CreateSolidBrush(RGB(0, 255, 0));
	nodeBrushDefault.CreateSolidBrush(RGB(255, 255, 255));
	linePen.CreatePen(PS_SOLID, 2, (COLORREF)RGB(0, 0, 0));
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGraphProjectDlg::OnPaint()
{
	
	if (IsIconic())
	{
		CClientDC dc(this); // device context for painting

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
	
		CDialogEx::OnPaint();
	}

	
	CString c;
	CClientDC pDC(this);

	// create backup buffer to draw into memory
	CDC dcBackBuffer; 
	dcBackBuffer.CreateCompatibleDC(&pDC);
	
	// create bitmap with size of client window 
	CBitmap bitmapBuffer;
	bitmapBuffer.CreateCompatibleBitmap(&pDC, win.Width(), win.Height());
	dcBackBuffer.SelectObject(&bitmapBuffer);

	int x, y;
	dcBackBuffer.FillSolidRect(&win, RGB(240, 240, 240));
	dcBackBuffer.Rectangle(&drawArea); // draw rectangle for draw area
	
	// update size of node list
	int size = (int)Nodes.size();
	numOfNodes = size;

	// draw nodes
	for (int i = 0; i < size; i++) {
		x = Nodes[i]->point.x;
		y = Nodes[i]->point.y;
		if (mode == MoveNode && movedNode == Nodes[i]) // paint selected node with red brush
			dcBackBuffer.SelectObject(nodeBrushRed);
		else if (mode == AddAdj && selectedNode == Nodes[i]) // paint selected node with green brush
			dcBackBuffer.SelectObject(nodeBrushGreen);
		else if (mode == EditNode && selectedNode == Nodes[i]) // paint selected node with red brush
			dcBackBuffer.SelectObject(nodeBrushRed);
		else // general node, not selected
			dcBackBuffer.SelectObject(nodeBrushDefault);

		Nodes[i]->Draw(&dcBackBuffer); // use draw to paint according to shape type (circle, rectangle or triangle)

		// draw number inside of node
		c.Format(_T("%d"), i + 1);
		dcBackBuffer.SetBkMode(TRANSPARENT);
		dcBackBuffer.DrawText(c, CRect(x - 10, y - 10, x + 10, y + 10), DT_CENTER);
	}

	// update edge size
	size = (int)Edges.size();
	numOfEdges = size;

	dcBackBuffer.SelectObject(linePen); // select pen for line

	// draw edges
	for (int i = 0; i < size; i++) {
		dcBackBuffer.MoveTo(Edges[i]->left->point);
		dcBackBuffer.LineTo(Edges[i]->right->point);
	}

	// copy bitmap to original device context
	pDC.BitBlt(0, 0, win.Width(), win.Height(), &dcBackBuffer, 0, 0, SRCCOPY);

	// update number of nodes/edges variables in their respective edit boxes
	UpdateData(FALSE);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGraphProjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




HBRUSH CGraphProjectDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}


void CGraphProjectDlg::OnBnClickedRadioAddAdj()
{
	mode = AddAdj;
	selectedNode = NULL;
	InvalidateRect(&drawArea);
}

void CGraphProjectDlg::OnBnClickedRadioRemoveNode()
{
	mode = RemoveNode;
	selectedNode = NULL;
	InvalidateRect(&drawArea);
}

void CGraphProjectDlg::OnBnClickedRadioAddNodes()
{
	mode = AddNode;
	selectedNode = NULL;
	InvalidateRect(&drawArea);
}

void CGraphProjectDlg::OnBnClickedRadioMoveNodes()
{
	mode = MoveNode;
	selectedNode = NULL;
	InvalidateRect(&drawArea);
}

void CGraphProjectDlg::OnBnClickedRadioEditNode()
{
	mode = EditNode;
	selectedNode = NULL;
	InvalidateRect(&drawArea);
}



void CGraphProjectDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (mode == AddNode)
		if (!drawAreaOffset.PtInRect(point)) // check if current point it inside of offset rectangle
			return;

	/*
		checks if point has landed on existing node
		and returns iterator to it, otherwise returns Nodes.end() = end of vector
	*/
	vector<Node *>::iterator t = nodeSearch(point);

	switch (mode) {
	case AddNode:
		if (t != Nodes.end()) // if did not reach end of vector
			AfxMessageBox(_T("Node already exists!"));
		else { // reached end of vector
			COLORREF c = nodeColorControl.GetColor();
			if (nodeType.GetCurSel() == 0) // circle
				Nodes.push_back(new nodeEllipse(point,c));
			else if (nodeType.GetCurSel() == 1) // rectangle
				Nodes.push_back(new nodeRectangle(point,c));
			else // triangle
				Nodes.push_back(new nodeTriangle(point,c));
		}
		break;
	case AddAdj:
		if (selectedNode) { 
			if (t != Nodes.end()) { // select second node after user has chosen first one
				if (selectedNode == *t) // check if same node
					AfxMessageBox(_T("Same node, edge not added!"));
				else if (edgeContains(selectedNode, *t) != Edges.end()) // check if edge exists
					AfxMessageBox(_T("Edge already exists!"));
				else // add new edge
					Edges.push_back(new Edge(selectedNode, *t));

				selectedNode = NULL;
			}
		}
		else // select node user clicked on
			if (t != Nodes.end())
				selectedNode = *t;
			else
				selectedNode = NULL;
		break;
	case MoveNode: 
		if (t != Nodes.end())
			movedNode = *t;
		else
			movedNode = NULL;
		break;
	case RemoveNode:
		if (t != Nodes.end()) {
			deleteEdges(*t);
			delete *t;
			Nodes.erase(t);
		}
		break;
	case EditNode:
		if (t != Nodes.end()) 
			selectedNode = *t;
		else
			selectedNode = NULL;
	}
	InvalidateRect(drawArea);
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CGraphProjectDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CString coords;
	coords.Format(_T("X = %04d, Y = %04d"), point.x, point.y);
	SetDlgItemText(IDC_STATIC_COORDS, coords);


	if (mode == MoveNode && movedNode) {
		if (drawAreaOffset.PtInRect(point)) {
			movedNode->point = point;
			InvalidateRect(&drawArea);
		}
	}
	
	//CClientDC dc(this);
	//CFont font;
	//font.CreatePointFont(90, _T("Arial"));
	//dc.SelectObject(&font);
	//dc.SetTextColor(RGB(0, 0, 102));
	//dc.SetBkColor(RGB(240, 240, 240));
	//dc.TextOut(0, win.Height() - 20, coords);
	CDialogEx::OnMouseMove(nFlags, point);
}


void CGraphProjectDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	movedNode = NULL;
	InvalidateRect(&drawArea);
	CDialogEx::OnLButtonUp(nFlags, point);
}


// checks if a couple of nodes exist in edge vector
vector<Edge *>::iterator CGraphProjectDlg::edgeContains(const Node *left, const Node* right) {
	vector<Edge *>::iterator e;

	for (e = Edges.begin(); e != Edges.end(); e++) {
		if ((left == (*e)->left && right == (*e)->right)
			|| (right == (*e)->left && left == (*e)->right))
			return e;
	}

	return e;
}

// searches for node using current point
vector<Node*>::iterator CGraphProjectDlg::nodeSearch(const CPoint &p) {
	CRect nodeArea;
	vector<Node*>::iterator t;
	for (t = Nodes.begin(); t != Nodes.end(); t++) {
		nodeArea.SetRect((*t)->point.x - SHAPE_AREA, (*t)->point.y - SHAPE_AREA,
			(*t)->point.x + SHAPE_AREA, (*t)->point.y + SHAPE_AREA);
		if ((*t)->PtInShape(p))
			return t;
	}
	return t;
}

// deletes all edges containing given node
void CGraphProjectDlg::deleteEdges(const Node *nd) {
	vector<Edge *>::iterator t = Edges.begin();

	while (t != Edges.end())
	{
		if ((*t)->left == nd || (*t)->right == nd) {
			delete *t;
			t = Edges.erase(t);
		}
		else
			t++;
	}
}

void CGraphProjectDlg::OnBnClickedComplementGraph()
{
	vector<Edge *>::iterator e;
	int size = (int)Nodes.size();
	for (int i = 0; i < size; i++) {
		for (int j = i + 1; j < size; j++) {
			e = edgeContains(Nodes[i], Nodes[j]);
			if (e != Edges.end()) {
				delete *e;
				Edges.erase(e);
			}
			else
				Edges.push_back(new Edge(Nodes[i], Nodes[j]));
		}
	}
	InvalidateRect(&drawArea);
}



void CGraphProjectDlg::OnCbnSelchangeComboNodeSelection()
{
	nodeType.GetLBText(nodeType.GetCurSel(), nodeTypeString);
	UpdateData(FALSE);
}

void CGraphProjectDlg::OnBnClickedMfcnodecolorbutton()
{
	if (mode == EditNode && selectedNode) {
		selectedNode->borderColor = nodeColorControl.GetColor();
		InvalidateRect(&drawArea);
	}
}

// released all memory of edges + nodes vectors
void CGraphProjectDlg::Clear() {
	vector<Node *>::iterator t;
	vector<Edge *>::iterator e;
	for (t = Nodes.begin(); t != Nodes.end(); t++)
		delete *t;
	Nodes.clear();

	for (e = Edges.begin(); e != Edges.end(); e++)
		delete *e;
	Edges.clear();
}



void CGraphProjectDlg::OnBnClickedButtonSave()
{
	CFileDialog dlg(FALSE, _T("grp"), _T("*.grp"));
	CString filename;
	int result = (int)dlg.DoModal();

	if (result == IDOK)
		filename = dlg.GetPathName();

	CFile f;

	if (f.Open(filename, CFile::modeCreate | CFile::modeWrite)) {
		CArchive ar(&f, CArchive::store);
		int size = (int)Nodes.size();

		ar << size;
		for (int i = 0; i < size; i++) {
			Nodes[i]->Serialize(ar);
		}
		
		size = (int)Edges.size();

		ar << size;
		for (int i = 0; i < size; i++) {
			ar << Edges[i]->left->id << Edges[i]->right->id;
		}

		ar.Close();
		f.Close();
	}

	
}


void CGraphProjectDlg::OnBnClickedButtonLoad()
{
	CFileDialog dlg(TRUE, _T("grp"), _T("*.grp"));
	CString filename;

	int res = (int)dlg.DoModal();
	if (res == IDOK)
		filename = dlg.GetPathName();

	CFile f;

	if (f.Open(filename, CFile::modeRead)) {
		Clear();
		CArchive ar(&f, CArchive::load);

		int size;
		ar >> size;
		Nodes.resize(size);

		for (int i = 0; i < size; i++) {
			int type;
			ar >> type;

			switch (type) {
			case 0:
				Nodes[i] = new nodeEllipse();
				Nodes[i]->Serialize(ar);
				break;
			case 1:
				Nodes[i] = new nodeRectangle();
				Nodes[i]->Serialize(ar);
				break;
			case 2:
				Nodes[i] = new nodeTriangle();
				Nodes[i]->Serialize(ar);
				break;
			}
		}

		ar >> size;
		Edges.resize(size);

		for (int i = 0; i < size; i++) {
			int leftID, rightID;
			Node *left = NULL, *right = NULL;

			ar >> leftID >> rightID;

			for (int j = 0; Nodes.size(); j++) {
				
				if (leftID == Nodes[j]->id)
					left = Nodes[j];
				if (rightID == Nodes[j]->id)
					right = Nodes[j];

				if (left && right)
					break;
			}

			Edges[i] = new Edge(left, right);
		}

		ar.Close();
		f.Close();

	}
	InvalidateRect(&drawArea);
}



BOOL CGraphProjectDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}


void CGraphProjectDlg::OnBnClickedButtonClear()
{
	Clear();
	InvalidateRect(&drawArea);
}

