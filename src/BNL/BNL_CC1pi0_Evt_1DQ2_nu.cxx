// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "BNL_CC1pi0_Evt_1DQ2_nu.h"

// The constructor
BNL_CC1pi0_Evt_1DQ2_nu::BNL_CC1pi0_Evt_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
  
  measurementName = "BNL_CC1pi0_Evt_1DQ2_nu";
  plotTitles = "; Q^{2}_{CC#pi} (GeV^{2}); Number of events";
  EnuMin = 0;
  EnuMax = 6.0;
  isDiag = true;
  isRawEvents = true;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/BNL/CC1pi0_on_n/BNL_CC1pi0_on_n_noEvents_q2_noWcut_bin_firstQ2gone.txt");
  this->SetupDefaultHist();

  // set Poisson errors on dataHist (scanned does not have this)
  // Simple counting experiment here
  for (int i = 0; i < dataHist->GetNbinsX() + 1; i++) {
    dataHist->SetBinError(i+1, sqrt(dataHist->GetBinContent(i+1)));
  }

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar = StatUtils::GetInvert(fullcovar);

  this->scaleFactor = this->eventHist->Integral("width")/(nevents+0.)*16./8.;
};


void BNL_CC1pi0_Evt_1DQ2_nu::FillEventVariables(FitEvent *event) {
  
  // set up the 4-vectors from NEUT
  TLorentzVector Pnu = event->PartInfo(0)->fP;
  TLorentzVector Pp;
  TLorentzVector Ppi0;
  TLorentzVector Pmu;

  // Loop over the particle stack to find relevant particles 
  // start at 2 because 0=nu, 1=nucleon, by NEUT default
  for (UInt_t j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 111) {
      Ppi0 = event->PartInfo(j)->fP;
    } else if (PID == 2212) {
      Pp = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;
    }
  }

  //double hadMass = FitUtils::MpPi(Pp, Ppi0);
    
  // no W cut on BNL CC1pi0 
  double q2CCpi0 = FitUtils::Q2CC1pi0rec(Pnu, Pmu, Ppi0);

  this->X_VAR = q2CCpi0;

  return;
};



bool BNL_CC1pi0_Evt_1DQ2_nu::isSignal(FitEvent *event) {

  if ((event->PartInfo(0))->fPID != 14) return false;

  if (((event->PartInfo(0))->fP.E() < this->EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > this->EnuMax*1000.)) return false; 

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false; 

  int pi0Cnt = 0;
  int lepCnt = 0;
  int protonCnt = 0;

  for (UInt_t j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 13) {
      lepCnt++;
    } else if (PID == 111) {
      pi0Cnt++;
    } else if (PID == 2212) {
      protonCnt++;
    } else {
      return false; // require only three prong events! (allow photons?)
    }
  }

  // don't think there's away of implementing spectator proton cuts in NEUT?
  // 100 MeV or larger protons

  if (pi0Cnt != 1) return false;
  if (lepCnt != 1) return false;
  if (protonCnt != 1) return false;

  return true;
}
