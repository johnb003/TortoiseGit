// TortoiseGitMerge - a Diff/Patch program

// Copyright (C) 2013-2014 - TortoiseGit
// Copyright (C) 2006-2010, 2012-2013 - TortoiseSVN

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#include "stdafx.h"
#include "TortoiseMerge.h"
#include "DirFileEnum.h"
#include "AppUtils.h"
#include "PathUtils.h"
#include "SetMainPage.h"
#include "SysInfo.h"

// CSetMainPage dialog

IMPLEMENT_DYNAMIC(CSetMainPage, CPropertyPage)
CSetMainPage::CSetMainPage()
	: CPropertyPage(CSetMainPage::IDD)
	, m_bBackup(FALSE)
	, m_bFirstDiffOnLoad(FALSE)
	, m_bFirstConflictOnLoad(FALSE)
	, m_nTabSize(0)
	, m_bIgnoreEOL(FALSE)
	, m_bOnePane(FALSE)
	, m_bViewLinenumbers(FALSE)
	, m_bReloadNeeded(FALSE)
	, m_bCaseInsensitive(FALSE)
	, m_bUTF8Default(FALSE)
	, m_bAutoAdd(TRUE)
	, m_nMaxInline(3000)
	, m_dwFontSize(0)
{
	m_regBackup = CRegDWORD(_T("Software\\TortoiseGitMerge\\Backup"));
	m_regFirstDiffOnLoad = CRegDWORD(_T("Software\\TortoiseGitMerge\\FirstDiffOnLoad"), TRUE);
	m_regFirstConflictOnLoad = CRegDWORD(_T("Software\\TortoiseGitMerge\\FirstConflictOnLoad"), TRUE);
	m_regTabSize = CRegDWORD(_T("Software\\TortoiseGitMerge\\TabSize"), 4);
	m_regIgnoreEOL = CRegDWORD(_T("Software\\TortoiseGitMerge\\IgnoreEOL"), TRUE);
	m_regOnePane = CRegDWORD(_T("Software\\TortoiseGitMerge\\OnePane"));
	m_regViewLinenumbers = CRegDWORD(_T("Software\\TortoiseGitMerge\\ViewLinenumbers"), 1);
	m_regFontName = CRegString(_T("Software\\TortoiseGitMerge\\LogFontName"), _T("Courier New"));
	m_regFontSize = CRegDWORD(_T("Software\\TortoiseGitMerge\\LogFontSize"), 10);
	m_regCaseInsensitive = CRegDWORD(_T("Software\\TortoiseGitMerge\\CaseInsensitive"), FALSE);
	m_regUTF8Default = CRegDWORD(_T("Software\\TortoiseGitMerge\\UseUTF8"), FALSE);
	m_regAutoAdd = CRegDWORD(_T("Software\\TortoiseGitMerge\\AutoAdd"), TRUE);
	m_regMaxInline = CRegDWORD(_T("Software\\TortoiseGitMerge\\InlineDiffMaxLineLength"), 3000);
	m_regUseRibbons = CRegDWORD(L"Software\\TortoiseGitMerge\\UseRibbons", TRUE);
	m_regUseTaskDialog = CRegDWORD(L"Software\\TortoiseGitMerge\\UseTaskDialog", TRUE);

	m_bBackup = m_regBackup;
	m_bFirstDiffOnLoad = m_regFirstDiffOnLoad;
	m_bFirstConflictOnLoad = m_regFirstConflictOnLoad;
	m_nTabSize = m_regTabSize;
	m_bIgnoreEOL = m_regIgnoreEOL;
	m_bOnePane = m_regOnePane;
	m_bViewLinenumbers = m_regViewLinenumbers;
	m_bCaseInsensitive = m_regCaseInsensitive;
	m_bUTF8Default = m_regUTF8Default;
	m_bAutoAdd = m_regAutoAdd;
	m_nMaxInline = m_regMaxInline;
	m_bUseRibbons = m_regUseRibbons;
	m_bUseTaskDialog = CTaskDialog::IsSupported() && (DWORD)m_regUseTaskDialog;
}

CSetMainPage::~CSetMainPage()
{
}

void CSetMainPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_BACKUP, m_bBackup);
	DDX_Check(pDX, IDC_FIRSTDIFFONLOAD, m_bFirstDiffOnLoad);
	DDX_Check(pDX, IDC_FIRSTCONFLICTONLOAD, m_bFirstConflictOnLoad);
	DDX_Text(pDX, IDC_TABSIZE, m_nTabSize);
	DDV_MinMaxInt(pDX, m_nTabSize, 1, 1000);
	DDX_Check(pDX, IDC_IGNORELF, m_bIgnoreEOL);
	DDX_Check(pDX, IDC_ONEPANE, m_bOnePane);
	DDX_Control(pDX, IDC_FONTSIZES, m_cFontSizes);
	m_dwFontSize = (DWORD)m_cFontSizes.GetItemData(m_cFontSizes.GetCurSel());
	if ((m_dwFontSize==0)||(m_dwFontSize == -1))
	{
		CString t;
		m_cFontSizes.GetWindowText(t);
		m_dwFontSize = _ttoi(t);
	}
	DDX_Control(pDX, IDC_FONTNAMES, m_cFontNames);
	DDX_Check(pDX, IDC_LINENUMBERS, m_bViewLinenumbers);
	DDX_Check(pDX, IDC_CASEINSENSITIVE, m_bCaseInsensitive);
	DDX_Check(pDX, IDC_UTF8DEFAULT, m_bUTF8Default);
	DDX_Check(pDX, IDC_AUTOADD, m_bAutoAdd);
	DDX_Text(pDX, IDC_MAXINLINE, m_nMaxInline);
	DDX_Check(pDX, IDC_USERIBBONS, m_bUseRibbons);
	DDX_Check(pDX, IDC_USETASKDIALOG, m_bUseTaskDialog);
}

void CSetMainPage::SaveData()
{
	m_regBackup = m_bBackup;
	m_regFirstDiffOnLoad = m_bFirstDiffOnLoad;
	m_regFirstConflictOnLoad = m_bFirstConflictOnLoad;
	m_regTabSize = m_nTabSize;
	m_regIgnoreEOL = m_bIgnoreEOL;
	m_regOnePane = m_bOnePane;
	m_regFontName = m_sFontName;
	m_regFontSize = m_dwFontSize;
	m_regViewLinenumbers = m_bViewLinenumbers;
	m_regCaseInsensitive = m_bCaseInsensitive;
	m_regUTF8Default = m_bUTF8Default;
	m_regAutoAdd = m_bAutoAdd;
	m_regMaxInline = m_nMaxInline;
	m_regUseRibbons = m_bUseRibbons;
	m_regUseTaskDialog = m_bUseTaskDialog;
}

BOOL CSetMainPage::OnApply()
{
	UpdateData();
	if (m_cFontNames.GetSelFont())
		m_sFontName = m_cFontNames.GetSelFont()->m_strName;
	else
		m_sFontName = m_regFontName;
	SaveData();
	SetModified(FALSE);
	return CPropertyPage::OnApply();
}

BOOL CSetMainPage::OnInitDialog()
{
	CMFCFontComboBox::m_bDrawUsingFont = true;

	CPropertyPage::OnInitDialog();

	m_bBackup = m_regBackup;
	m_bFirstDiffOnLoad = m_regFirstDiffOnLoad;
	m_bFirstConflictOnLoad = m_regFirstConflictOnLoad;
	m_nTabSize = m_regTabSize;
	m_bIgnoreEOL = m_regIgnoreEOL;
	m_bOnePane = m_regOnePane;
	m_sFontName = m_regFontName;
	m_dwFontSize = m_regFontSize;
	m_bViewLinenumbers = m_regViewLinenumbers;
	m_bCaseInsensitive = m_regCaseInsensitive;
	m_bUTF8Default = m_regUTF8Default;
	m_bAutoAdd = m_regAutoAdd;
	m_nMaxInline = m_regMaxInline;
	m_bUseRibbons = m_regUseRibbons;
#if _WIN64
	if (SysInfo::Instance().IsXP())
	{
		DialogEnableWindow(IDC_USERIBBONS, FALSE);
		m_bUseRibbons = FALSE;
	}
#endif
	m_bUseTaskDialog = CTaskDialog::IsSupported() && (DWORD)m_regUseTaskDialog;

	DialogEnableWindow(IDC_FIRSTCONFLICTONLOAD, m_bFirstDiffOnLoad);
	DialogEnableWindow(IDC_USETASKDIALOG, CTaskDialog::IsSupported());

	CString temp;
	int count = 0;
	for (int i=6; i<32; i=i+2)
	{
		temp.Format(_T("%d"), i);
		m_cFontSizes.AddString(temp);
		m_cFontSizes.SetItemData(count++, i);
	}
	BOOL foundfont = FALSE;
	for (int i=0; i<m_cFontSizes.GetCount(); i++)
	{
		if (m_cFontSizes.GetItemData(i) == m_dwFontSize)
		{
			m_cFontSizes.SetCurSel(i);
			foundfont = TRUE;
		}
	}
	if (!foundfont)
	{
		temp.Format(_T("%lu"), m_dwFontSize);
		m_cFontSizes.SetWindowText(temp);
	}
	m_cFontNames.Setup(DEVICE_FONTTYPE|RASTER_FONTTYPE|TRUETYPE_FONTTYPE, 1, FIXED_PITCH);
	m_cFontNames.SelectFont(m_sFontName);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CSetMainPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BACKUP, &CSetMainPage::OnModified)
	ON_BN_CLICKED(IDC_IGNORELF, &CSetMainPage::OnModifiedWithReload)
	ON_BN_CLICKED(IDC_ONEPANE, &CSetMainPage::OnModified)
	ON_BN_CLICKED(IDC_FIRSTDIFFONLOAD, &CSetMainPage::OnModified)
	ON_BN_CLICKED(IDC_FIRSTCONFLICTONLOAD, &CSetMainPage::OnModified)
	ON_BN_CLICKED(IDC_LINENUMBERS, &CSetMainPage::OnModified)
	ON_EN_CHANGE(IDC_TABSIZE, &CSetMainPage::OnModified)
	ON_CBN_SELCHANGE(IDC_FONTSIZES, &CSetMainPage::OnModified)
	ON_CBN_SELCHANGE(IDC_FONTNAMES, &CSetMainPage::OnModified)
	ON_BN_CLICKED(IDC_CASEINSENSITIVE, &CSetMainPage::OnModified)
	ON_BN_CLICKED(IDC_UTF8DEFAULT, &CSetMainPage::OnModified)
	ON_BN_CLICKED(IDC_AUTOADD, &CSetMainPage::OnModified)
	ON_EN_CHANGE(IDC_MAXINLINE, &CSetMainPage::OnModifiedWithReload)
	ON_BN_CLICKED(IDC_USERIBBONS, &CSetMainPage::OnModified)
	ON_BN_CLICKED(IDC_USETASKDIALOG, &CSetMainPage::OnModified)
END_MESSAGE_MAP()


// CSetMainPage message handlers

void CSetMainPage::OnModified()
{
	UpdateData();
	SetModified();
	DialogEnableWindow(IDC_FIRSTCONFLICTONLOAD, m_bFirstDiffOnLoad);
}

void CSetMainPage::OnModifiedWithReload()
{
	m_bReloadNeeded = TRUE;
	SetModified();
}

void CSetMainPage::OnBnClickedWhitespace()
{
	m_bReloadNeeded = TRUE;
	SetModified();
}

BOOL CSetMainPage::DialogEnableWindow(UINT nID, BOOL bEnable)
{
	CWnd * pwndDlgItem = GetDlgItem(nID);
	if (pwndDlgItem == NULL)
		return FALSE;
	if (bEnable)
		return pwndDlgItem->EnableWindow(bEnable);
	if (GetFocus() == pwndDlgItem)
	{
		SendMessage(WM_NEXTDLGCTL, 0, FALSE);
	}
	return pwndDlgItem->EnableWindow(bEnable);
}
