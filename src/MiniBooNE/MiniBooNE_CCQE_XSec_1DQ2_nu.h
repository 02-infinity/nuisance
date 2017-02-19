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

#ifndef MINIBOONE_CCQE_XSEC_1DQ2_NU_H_SEEN
#define MINIBOONE_CCQE_XSEC_1DQ2_NU_H_SEEN

#include "Measurement1D.h"

//********************************************************************
class MiniBooNE_CCQE_XSec_1DQ2_nu : public Measurement1D {
//********************************************************************

public:

  MiniBooNE_CCQE_XSec_1DQ2_nu(nuiskey samplekey);
  virtual ~MiniBooNE_CCQE_XSec_1DQ2_nu() {};

  void FillEventVariables(FitEvent *event);
  // void FillHistograms();
  // void Write(std::string drawOpt);
  bool isSignal(FitEvent *event);
  // void ScaleEvents();
  // void ApplyNormScale(double norm);
  // void ResetAll();
  void FillExtraHistograms(MeasurementVariableBox* vars, double weight = 1.0);


  TH1D* fMCHist_CCQELIKE[61]; ///<! Plots in CCQELike mode to tag PDG of the background

private:
  double q2qe; ///<! X_Variable
  bool bad_particle; ///<! Used in CCQELike mode to tag events without nucleons, muons or photons.
  bool ccqelike; ///<! Flag for running in CCQELike mode
  TH1D* fDataHist_CCQELIKE; ///<! CCQELike data contribution
  TH1D* fMCHist_CCQELIKETOTAL;
};

#endif
