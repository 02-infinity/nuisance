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

#include "MiniBooNE_CC1pip_XSec_2DQ2Enu_nu.h"

//******************************************************************** 
MiniBooNE_CC1pip_XSec_2DQ2Enu_nu::MiniBooNE_CC1pip_XSec_2DQ2Enu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//******************************************************************** 
  
  fName = "MiniBooNE_CC1pip_XSec_2DQ2Enu_nu";
  fPlotTitles = "; E_{#nu} (MeV); Q^{2} (MeV^{2}/c^{4}); d#sigma(E_{#nu})/dQ^{2} (cm^{2}/(MeV^{2}/c^{4})/CH_{2})";
  EnuMin = 0.5;
  EnuMax = 2.0;
  fIsDiag = true;
  fNormError = 0.107;
  Measurement2D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MiniBooNE/CC1pip/ccpipXSecs.root", std::string("QSQVENUXSec"));//data comes in .root file, yes!
  this->SetupDefaultHist();

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fullcovar);

  // Calculates a flux averaged cross-section from (Evt("width")/Flux("width")) * 14.08/6.0
  this->fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents)*(14.08);

};

void MiniBooNE_CC1pip_XSec_2DQ2Enu_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = event->PartInfo(0)->fP;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (unsigned int j = 2; j < event->Npart(); ++j){
    if (!event->PartInfo(j)->fIsAlive || event->PartInfo(j)->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211) {
      Ppip = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = event->PartInfo(j)->fP;
    }
  }

  double Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppip)*1000.;
  double Q2 = FitUtils::Q2CC1piprec(Pnu, Pmu, Ppip)*1E6;

  fXVar = Enu;
  fYVar = Q2;

  return;
};

//******************************************************************** 
bool MiniBooNE_CC1pip_XSec_2DQ2Enu_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  return SignalDef::isCC1pip_MiniBooNE(event, EnuMin, EnuMax);
}
