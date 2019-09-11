
// PCLDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PCLDemo.h"
#include "PCLDemoDlg.h"
#include "afxdialogex.h"


#include <opencv2/opencv.hpp> 
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_cloud.h> 
#include <boost/thread/thread.hpp>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/visualization/pcl_visualizer.h>  


#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

using namespace pcl;
using namespace std;

typedef pcl::PointXYZ PointT;
typedef pcl::PointCloud<PointT> PointCloudT;


vtkSmartPointer<vtkRenderWindow> m_win;
vtkSmartPointer<vtkRenderWindowInteractor> m_iren;

boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer", false));

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CPCLDemoDlg 对话框



CPCLDemoDlg::CPCLDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PCLDEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPCLDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPCLDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPENCLOUD, &CPCLDemoDlg::OnBnClickedOpencloud)
END_MESSAGE_MAP()


// CPCLDemoDlg 消息处理程序

BOOL CPCLDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//==========================将viewer窗口结合到MFC图像控件上===========================
	CRect rect;
	m_iren = vtkRenderWindowInteractor::New();//重新申请地址
	m_win = viewer->getRenderWindow();//获取viewer的句柄
	GetDlgItem(IDC_FRAME)->GetClientRect(&rect);//获取图像控件的实际大小
	m_win->SetSize(rect.right - rect.left, rect.bottom - rect.top);//将viewer大小设置为图像控件大小
	m_win->SetParentId(GetDlgItem(IDC_FRAME)->m_hWnd);//将vtk窗口结合到MFC窗口中
	viewer->resetCamera();
	m_iren->SetRenderWindow(m_win);
	viewer->createInteractor();//由于初始化设置为false，该处重新创建PCL风格的Interactor
	m_win->Render();
	//========================================================================================

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPCLDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPCLDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPCLDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPCLDemoDlg::OnBnClickedOpencloud()
{
	// TODO: 在此添加控件通知处理程序代码
	PointCloudT::Ptr cloud(new PointCloudT);
	pcl::io::loadPCDFile("cloud.pcd", *cloud);
	viewer->addPointCloud(cloud);
}
