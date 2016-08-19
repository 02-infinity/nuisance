// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NUISANCE.
*
*    NUISANCE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NUISANCE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef BEBC_CC1PPIP_XSEC_1DENU_NU_H_SEEN
#define BEBC_CC1PPIP_XSEC_1DENU_NU_H_SEEN

#include "Measurement1D.h"

class BEBC_CC1ppip_XSec_1DEnu_nu : public Measurement1D {

public:
  BEBC_CC1ppip_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile);
  virtual ~BEBC_CC1ppip_XSec_1DEnu_nu() {};

  void FillEventVariables(FitEvent *event);
  //void ScaleEvents();
  bool isSignal(FitEvent *event);
  //void FillHistograms();


 private:

};
  
#endif
