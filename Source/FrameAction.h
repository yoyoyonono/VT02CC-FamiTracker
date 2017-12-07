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


#pragma once

#include "Action.h"
#include "FrameEditorTypes.h"		// // //
#include "IntRange.h"		// // //

class CFrameEditor;		// // //
class CFamiTrackerView;
class CMainFrame;
class CFamiTrackerDoc;
class CFrameClipData;		// // //

/*
	\brief A structure responsible for recording the cursor and selection state of the frame
	editor for use by frame actions. Uses CFamiTrackerView since the frame editor class is not
	independent.
*/
struct CFrameEditorState		// TODO maybe merge this with CPatternEditorState
{
	/*!	\brief Constructor of the frame editor state.
		\details On construction, the object retrieves the current state of the frame editor
		immediately. Once created, a state object remains constant and can be applied back to the
		frame editor as many times as desired.
		\param pView Pointer to the tracker view.
		\param Track The track number. */
	CFrameEditorState(const CFamiTrackerView *pEditor, int Track);

	/*!	\brief Applies the state to a frame editor.
		\param pView Pointer to the tracker view. */
	void ApplyState(CFamiTrackerView *pView) const;
	
	/*!	\brief Obtains the first selected frame.
		\return Starting frame index. */
	int GetFrameStart() const;
	/*!	\brief Obtains the last selected frame.
		\return Ending channel index. */
	int GetFrameEnd() const;
	/*!	\brief Obtains the first selected channel.
		\return Starting frame index. */
	int GetChanStart() const;
	/*!	\brief Obtains the last selected channel.
		\return Ending channel index. */
	int GetChanEnd() const;

	/*!	\brief The current track number at the time of the state's creation. */
	int Track;

	/*!	\brief The current cursor position at the time of the state's creation. */
	CFrameCursorPos Cursor;

	/*!	\brief The current selection position at the time of the state's creation. */
	CFrameSelection Selection;

	/*!	\brief Whether a selection is active at the time of the state's creation. */
	bool IsSelecting;

private:
	CFrameSelection OriginalSelection;
};

// Frame commands
class CFrameAction : public CAction
{
protected:
	CFrameAction() = default;		// // //
	virtual ~CFrameAction();

	void SaveUndoState(const CMainFrame &MainFrm) override;		// // //
	void SaveRedoState(const CMainFrame &MainFrm) override;		// // //
	void RestoreUndoState(CMainFrame &MainFrm) const override;		// // //
	void RestoreRedoState(CMainFrame &MainFrm) const override;		// // //
	void UpdateViews(CMainFrame &MainFrm) const override;		// // //

protected:
	static int ClipPattern(int Pattern);

	CIntRange<int> m_itFrames, m_itChannels;

protected:
	CFrameEditorState *m_pUndoState = nullptr, *m_pRedoState = nullptr;		// // //
};

// // // built-in frame action subtypes

class CFActionAddFrame : public CFrameAction
{
public:
	CFActionAddFrame() = default;
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
};

class CFActionRemoveFrame : public CFrameAction
{
public:
	CFActionRemoveFrame() = default;
	~CFActionRemoveFrame();
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
private:
	CFrameClipData *m_pRowClipData = nullptr;
};

class CFActionDuplicateFrame : public CFrameAction
{
public:
	CFActionDuplicateFrame() = default;
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
};

class CFActionCloneFrame : public CFrameAction
{
public:
	CFActionCloneFrame() = default;
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
};

class CFActionFrameCount : public CFrameAction
{
public:
	CFActionFrameCount(int Count) : m_iNewFrameCount(Count) { }
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
	bool Merge(const CAction &Other) override;		// // //
	void UpdateViews(CMainFrame &MainFrm) const override;
private:
	int m_iOldFrameCount, m_iNewFrameCount;
};

class CFActionSetPattern : public CFrameAction
{
public:
	CFActionSetPattern(int Pattern) : m_iNewPattern(Pattern) { }
	~CFActionSetPattern();
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
	bool Merge(const CAction &Other) override;		// // //
private:
	int m_iNewPattern;
	CFrameClipData *m_pClipData = nullptr;
};

class CFActionSetPatternAll : public CFrameAction
{
public:
	CFActionSetPatternAll(int Pattern) : m_iNewPattern(Pattern) { }
	~CFActionSetPatternAll();
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
	bool Merge(const CAction &Other) override;		// // //
private:
	int m_iNewPattern;
	CFrameClipData *m_pRowClipData = nullptr;
};

class CFActionChangePattern : public CFrameAction
{
public:
	CFActionChangePattern(int Offset) : m_iPatternOffset(Offset) { }
	~CFActionChangePattern();
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
	bool Merge(const CAction &Other) override;		// // //
private:
	int m_iPatternOffset;
	CFrameClipData *m_pClipData = nullptr;
	mutable bool m_bOverflow = false;
};

class CFActionChangePatternAll : public CFrameAction
{
public:
	CFActionChangePatternAll(int Offset) : m_iPatternOffset(Offset) { }
	~CFActionChangePatternAll();
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
	bool Merge(const CAction &Other) override;		// // //
private:
	int m_iPatternOffset;
	CFrameClipData *m_pRowClipData = nullptr;
	mutable bool m_bOverflow = false;
};

class CFActionMoveDown : public CFrameAction
{
public:
	CFActionMoveDown() = default;
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
};

class CFActionMoveUp : public CFrameAction
{
public:
	CFActionMoveUp() = default;
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
};

class CFActionClonePatterns : public CFrameAction		// // //
{
public:
	CFActionClonePatterns() = default;
	~CFActionClonePatterns();
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
private:
	int m_iOldPattern, m_iNewPattern;
	CFrameClipData *m_pClipData = nullptr;
};

class CFActionPaste : public CFrameAction
{
public:
	CFActionPaste(CFrameClipData *pData, int Frame, bool Clone) : m_pClipData(pData), m_iTargetFrame(Frame), m_bClone(Clone) { }
	~CFActionPaste();
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
	void UpdateViews(CMainFrame &MainFrm) const override;
private:
	CFrameClipData *m_pClipData = nullptr;
	int m_iTargetFrame;
	bool m_bClone;
};

class CFActionPasteOverwrite : public CFrameAction
{
public:
	CFActionPasteOverwrite(CFrameClipData *pData) : m_pClipData(pData) { }
	~CFActionPasteOverwrite();
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
private:
	CFrameClipData *m_pClipData = nullptr, *m_pOldClipData = nullptr;
	CFrameSelection m_TargetSelection;
};

class CFActionDropMove : public CFrameAction
{
public:
	CFActionDropMove(CFrameClipData *pData, int Frame) : m_pClipData(pData), m_iTargetFrame(Frame) { }
	~CFActionDropMove();
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
private:
	CFrameClipData *m_pClipData = nullptr;
	int m_iTargetFrame;
};

class CFActionDeleteSel : public CFrameAction
{
public:
	CFActionDeleteSel() = default;
	~CFActionDeleteSel();
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
	void UpdateViews(CMainFrame &MainFrm) const override;
private:
	CFrameClipData *m_pClipData = nullptr;
};

class CFActionMergeDuplicated : public CFrameAction
{
public:
	CFActionMergeDuplicated() = default;
	~CFActionMergeDuplicated();
private:
	bool SaveState(const CMainFrame &MainFrm) override;
	void Undo(CMainFrame &MainFrm) override;
	void Redo(CMainFrame &MainFrm) override;
private:
	CFrameClipData *m_pClipData = nullptr, *m_pOldClipData = nullptr;
};
