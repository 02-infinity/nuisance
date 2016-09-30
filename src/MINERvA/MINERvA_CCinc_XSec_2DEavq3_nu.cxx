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

#include "MINERvA_CCinc_XSec_2DEavq3_nu.h"

//********************************************************************   
MINERvA_CCinc_XSec_2DEavq3_nu::MINERvA_CCinc_XSec_2DEavq3_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//********************************************************************

  // Measurement Details
  fName = "MINERvA_CCinc_XSec_2DEavq3_nu";
  fPlotTitles = "; q_{3} (GeV); E_{avail} (GeV); d^{2}#sigma/dq_{3}dE_{avail} (cm^{2}/GeV^{2})";
  EnuMin = 2.;
  EnuMax = 6.;
  hadroncut = FitPar::Config().GetParB("MINERvA_CCinc_XSec_2DEavq3_nu.hadron_cut");
  useq3true = FitPar::Config().GetParB("MINERvA_CCinc_XSec_2DEavq3_nu.useq3true");
  splitMEC_PN_NN = FitPar::Config().GetParB("Modes.split_PN_NN");
  fNormError = 0.107;
  default_types = "FIX/FULL";
  allowed_types = "FIX,FREE,SHAPE/FULL,DIAG/MASK";
  Measurement2D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Binning for the 2D Histograms
  this->data_points_x = 7;
  this->data_points_y = 17;
  Double_t tempx[7] = {0.0, 0.2, 0.3, 0.4, 0.5, 0.6, 0.8};
  Double_t tempy[17] = {0.0, 0.02, 0.04, 0.06, 0.08, 0.10, 0.12, 0.14, 0.16, 0.20, 0.25, 0.30, 0.35, 0.40, 0.50, 0.60, 0.80};
  this->xBins = tempx;
  this->yBins = tempy;
  
  // Fill data and 1Dto2D Maps for covariance
  SetDataValuesFromText(FitPar::GetDataBase()+"/MINERvA/CCEavQ3/data_2D.txt", 1E-42);
  SetMapValuesFromText(FitPar::GetDataBase()+"/MINERvA/CCEavQ3/map_2D.txt");
  SetCovarMatrixFromChol(FitPar::GetDataBase()+"/MINERvA/CCEavQ3/covar_2D.txt", 67);

  // Data is in 1E-42 and so is the covariance, need to scale accordingly.
  (*this->fullcovar) *= 1E-16;
  (*this->covar) *= 1E16;
  
  // Set data errors from covariance matrix
  StatUtils::SetDataErrorFromCov(fDataHist, fullcovar, mapHist, 1E-38);
    
  // Setup mc Histograms
  SetupDefaultHist();
 
  // Set Scale Factor
  fScaleFactor = (this->fEventHist->Integral("width")*1E-42/(nevents+0.))/this->TotalIntegratedFlux(); 
};



//******************************************************************** 
void MINERvA_CCinc_XSec_2DEavq3_nu::FillEventVariables(FitEvent *event){
//********************************************************************

  // Set starting variables
  double Eav  = 0.0; // Energy Avaiable
  double q3   = 0.0; // Three momentum transfer
  double q0   = 0.0; // True Energy Transfer
    
  // Individual particle variables
  int PID  = 0;
  double Q2   = 0.0;
  TLorentzVector q = TLorentzVector(); // reset vector
  double Mmu = 0.0;
  double Emu = 0.0;
  double pmu = 0.0;
  double ThetaMu = 0.0;

  // Seperate MEC
  if (splitMEC_PN_NN){
    int npr = 0;
    int nne = 0;
    
    for (UInt_t j = 0; j < event->Npart(); j++){
      if ((event->PartInfo(j))->fIsAlive) continue;

      if (event->PartInfo(j)->fPID == 2212) npr++;
      else if (event->PartInfo(j)->fPID == 2112) nne++;
    }

    if (event->Mode == 2 and npr == 1 and nne == 1){
      event->Mode = 2;
      Mode = 2;

    } else if (event->Mode == 2 and npr == 0 and nne == 2){
      event->Mode = 3;
      Mode = 3;

    }
  }

  // Muon Variables -----------------
  // Muon should be in slot 2 or 3.
  for (UInt_t j = 2; j < event->Npart(); j++){

    // Skip dead particles
    PID = event->PartInfo(j)->fPID;

    // MUON
    if (PID == 13){
      
      // Set q from muon
      q = ((event->PartInfo(0))->fP - (event->PartInfo(j))->fP);
      q0 = q.E()/1000.0;
      
      // Other muon variables
      ThetaMu = (event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect());
      pmu = ((event->PartInfo(j))->fP.Vect().Mag())/1000.0;
      Emu = ((event->PartInfo(j))->fP.E())/1000.0;
      Mmu = ((event->PartInfo(j))->fP.Mag())/1000.0;  

      Enu_rec = Emu + q0;
      
      // Set Q2 from reconstruction method            
      Q2 = 2*Enu_rec * (Emu - pmu * cos(ThetaMu)) - Mmu*Mmu;
      
      // merge together for q3        
      q3 = q.Vect().Mag()/1000.0;
      if (!useq3true) q3 = sqrt( Q2 + q0*q0 );
      
      continue;
    }

    if (!(event->PartInfo(j))->fIsAlive) continue;
    if (event->PartInfo(j)->fStatus != 0) continue;

    // Eav Varible -----------------
    // P and pi+- Kinetic Energy
    if (PID == 2212 or PID ==  211 or PID == -211){
      Eav += FitUtils::T(event->PartInfo(j)->fP);
      
    // Total Energy of non-neutrons
    } else if (PID != 2112 and PID < 999 and PID != 22 and abs(PID) != 14){
      Eav += (event->PartInfo(j)->fP.E())/1000.0;
    }
  }

  // Set Hist Variables
  fXVar = q3;
  fYVar = Eav;

  return;
}

//******************************************************************** 
bool MINERvA_CCinc_XSec_2DEavq3_nu::isSignal(FitEvent *event){
//********************************************************************
  return SignalDef::isCCincLowRecoil_MINERvA(event, EnuMin, EnuMax, hadroncut);
}
