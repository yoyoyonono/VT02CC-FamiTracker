/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2014  Jonathan Liss
**
** 0CC-FamiTracker is (C) 2014-2017 HertzDevil
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
*/

#include "ModuleImportDlg.h"
#include "FamiTrackerDoc.h"
#include "SongData.h"		// // //

// CModuleImportDlg dialog

IMPLEMENT_DYNAMIC(CModuleImportDlg, CDialog)

CModuleImportDlg::CModuleImportDlg(CFamiTrackerDoc *pDoc)
	: CDialog(CModuleImportDlg::IDD, NULL), m_pImportedDoc(NULL), m_pDocument(pDoc)
{
}

CModuleImportDlg::~CModuleImportDlg()
{
	SAFE_RELEASE(m_pImportedDoc);
}

void CModuleImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CModuleImportDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CModuleImportDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CModuleImportDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CModuleImportDlg message handlers

BOOL CModuleImportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctlTrackList.SubclassDlgItem(IDC_TRACKS, this);

	int TrackCount = m_pImportedDoc->GetTrackCount();
	
	for (int i = 0; i < TrackCount; ++i) {
		CString str;
		str.Format(_T("#%02i %s"), i + 1, m_pImportedDoc->GetTrackTitle(i).c_str());		// // //
		m_ctlTrackList.AddString(str);
		m_ctlTrackList.SetCheck(i, 1);
	}

	CheckDlgButton(IDC_INSTRUMENTS, BST_CHECKED);
	CheckDlgButton(IDC_IMPORT_GROOVE, BST_CHECKED);		// // //
	CheckDlgButton(IDC_IMPORT_DETUNE, BST_UNCHECKED);		// // //

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CModuleImportDlg::OnBnClickedOk()
{
	if (!(ImportInstruments() && ImportGrooves() && ImportDetune() && ImportTracks()))		// // //
		AfxMessageBox(IDS_IMPORT_FAILED, MB_ICONERROR);

	// TODO another way to do this?
	m_pDocument->UpdateAllViews(NULL, UPDATE_PATTERN);
	m_pDocument->UpdateAllViews(NULL, UPDATE_FRAME);
	m_pDocument->UpdateAllViews(NULL, UPDATE_INSTRUMENT);

	OnOK();
}

void CModuleImportDlg::OnBnClickedCancel()
{
	OnCancel();
}

bool CModuleImportDlg::LoadFile(CString Path)		// // //
{	
	m_pImportedDoc = CFamiTrackerDoc::LoadImportFile(Path);

	// Check if load failed
	if (m_pImportedDoc == NULL)
		return false;

	// Check expansion chip match
	// // // import as superset of expansion chip configurations
	if (m_pImportedDoc->GetNamcoChannels() != m_pDocument->GetNamcoChannels()) {
		int Max = m_pImportedDoc->GetNamcoChannels();
		if (m_pDocument->GetNamcoChannels() > Max) Max = m_pDocument->GetNamcoChannels();
		m_pImportedDoc->SetNamcoChannels(Max, true);
		m_pDocument->SetNamcoChannels(Max, true);
		unsigned char Chip = m_pImportedDoc->GetExpansionChip() | m_pDocument->GetExpansionChip();
		m_pImportedDoc->SelectExpansionChip(Chip, true);
		m_pDocument->SelectExpansionChip(Chip, true);
	}
	if (m_pImportedDoc->GetExpansionChip() != m_pDocument->GetExpansionChip()) {
		unsigned char Chip = m_pImportedDoc->GetExpansionChip() | m_pDocument->GetExpansionChip();
		m_pImportedDoc->SelectExpansionChip(Chip, true);
		m_pDocument->SelectExpansionChip(Chip, true);
	}

	return true;
}

bool CModuleImportDlg::ImportInstruments()
{
	for (int i = 0; i < MAX_INSTRUMENTS; ++i)		// // //
		m_iInstrumentTable[i] = i;

	if (IsDlgButtonChecked(IDC_INSTRUMENTS) == BST_CHECKED)
		if (!m_pDocument->ImportInstruments(m_pImportedDoc, m_iInstrumentTable))
			return false;

	return true;
}

bool CModuleImportDlg::ImportGrooves()		// // //
{
	for (int i = 0; i < MAX_GROOVE; ++i)
		m_iGrooveMap[i] = i;

	if (IsDlgButtonChecked(IDC_IMPORT_GROOVE) == BST_CHECKED)
		if (!m_pDocument->ImportGrooves(m_pImportedDoc, m_iGrooveMap))
			return false;

	return true;
}

bool CModuleImportDlg::ImportDetune()		// // //
{
	if (IsDlgButtonChecked(IDC_IMPORT_DETUNE) == BST_CHECKED)
		if (!m_pDocument->ImportDetune(m_pImportedDoc))
			return false;

	return true;
}

bool CModuleImportDlg::ImportTracks()
{
	// // // ensure there are enough track slots
	unsigned count = 0;
	for (unsigned int i = 0; i < m_pImportedDoc->GetTrackCount(); ++i)
		if (m_ctlTrackList.GetCheck(i) == BST_CHECKED)
			if (++count + m_pDocument->GetTrackCount() > MAX_TRACKS)
				return false;

	// // // translate instruments and grooves in imported doc directly
	m_pImportedDoc->VisitSongs([this] (CSongData &song) {
		if (song.GetSongGroove())
			song.SetSongSpeed(m_iGrooveMap[song.GetSongSpeed()]);
		song.VisitPatterns([this] (CPatternData &pat) {
			pat.VisitRows([this] (stChanNote &note, unsigned) {
				// Translate instrument number
				if (note.Instrument < MAX_INSTRUMENTS)
					note.Instrument = m_iInstrumentTable[note.Instrument];
				// // // Translate groove commands
				for (int i = 0; i < MAX_EFFECT_COLUMNS; ++i)
					if (note.EffNumber[i] == EF_GROOVE && note.EffParam[i] < MAX_GROOVE)
						note.EffParam[i] = m_iGrooveMap[note.EffParam[i]];
			});
		});
	});

	// Import track
	for (unsigned int i = 0; i < m_pImportedDoc->GetTrackCount(); ++i)
		if (m_ctlTrackList.GetCheck(i) == BST_CHECKED)
			m_pDocument->ImportTrack(i, m_pImportedDoc);

	// Rebuild instrument list
	m_pDocument->ModifyIrreversible();		// // //
	m_pDocument->UpdateAllViews(NULL, UPDATE_INSTRUMENT);
	m_pDocument->UpdateAllViews(NULL, UPDATE_TRACK);		// // //

	return true;
}
