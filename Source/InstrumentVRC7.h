/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2014  Jonathan Liss
**
** 0CC-FamiTracker is (C) 2014-2018 HertzDevil
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

#include "Instrument.h"
#include <array>		// // //

class CInstrumentVRC7 : public CInstrument {
public:
	CInstrumentVRC7();
	std::unique_ptr<CInstrument> Clone() const override;
	bool	CanRelease() const override;

public:
	void		 SetPatch(unsigned int Patch);
	unsigned int GetPatch() const;
	void		 SetCustomReg(int Reg, unsigned char Value);		// // //
	unsigned char GetCustomReg(int Reg) const;		// // //

protected:
	void	CloneFrom(const CInstrument *pInst) override;		// // //

private:
	unsigned int m_iPatch = 0;
	std::array<unsigned char, 8> m_iRegs = { };		// // // Custom patch settings
};
