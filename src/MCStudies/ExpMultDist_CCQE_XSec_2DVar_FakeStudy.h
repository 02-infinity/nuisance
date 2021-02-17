// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#ifndef ExpMultDist_CCQE_XSec_2DVar_FakeStudy_H_SEEN
#define ExpMultDist_CCQE_XSec_2DVar_FakeStudy_H_SEEN
#include "Measurement2D.h"

//******************************************************************** 
class ExpMultDist_CCQE_XSec_2DVar_FakeStudy : public Measurement2D {
//******************************************************************** 

public:

  ExpMultDist_CCQE_XSec_2DVar_FakeStudy(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile);
  virtual ~ExpMultDist_CCQE_XSec_2DVar_FakeStudy() {};
  
  //! Grab info from event 
  void FillEventVariables(FitEvent *event);

  //! Define this samples signal 
  bool isSignal(FitEvent *nvect);
    
 private:
  
  double q2qe, Tmu, CosThetaMu;
  int plottype;
};
  
#endif
