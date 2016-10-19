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

#include "MiniBooNE_CC1pi0_XSec_1DEnu_nu.h"

// The constructor
MiniBooNE_CC1pi0_XSec_1DEnu_nu::MiniBooNE_CC1pi0_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
  
  fName = "MiniBooNE_CC1pi0_XSec_1DEnu_nu";
  fPlotTitles = "; E_{#nu} (GeV); #sigma(E_{#nu}) (cm^{2}/CH_{2})";
  EnuMin = 0.5;
  EnuMax = 2.;
  fIsDiag = false;
  fIsEnu1D = true;
  fNormError = 0.107;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MiniBooNE/CC1pi0/totalxsec_edit.txt");
  this->SetCovarMatrixFromCorrText(std::string(std::getenv("EXT_FIT"))+"/data/MiniBooNE/CC1pi0/totalxsec_covar.txt", this->fNDataPointsX);
  //this->SetCovarMatrix(FitPar::GetDataBase()+"/MiniBooNE/cc1pi0/totalxsec_covar.txt", this->fNDataPointsX-1);
  this->SetupDefaultHist();


  this->fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents)*(14.08);
};

void MiniBooNE_CC1pi0_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  // Set up the 4-vectors
  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pmu;
  TLorentzVector Ppi0;
  
  // Loop over the particle stack
  for (unsigned int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fNEUTStatusCode != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 111) {
      Ppi0 = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;
    }
  }
  
  double Enu = FitUtils::EnuCC1pi0rec(Pnu, Pmu, Ppi0);

  fXVar = Enu;

  return;
};

bool MiniBooNE_CC1pi0_XSec_1DEnu_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi0_MiniBooNE(event, EnuMin, EnuMax);
}
