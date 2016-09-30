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

#include "ANL_CC1ppip_Evt_1Dphi_nu.h"

// The constructor
ANL_CC1ppip_Evt_1Dphi_nu::ANL_CC1ppip_Evt_1Dphi_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) {
  
  fName = "ANL_CC1ppip_Evt_1Dphi_nu";
  fPlotTitles = "; #phi_{Adler}; Number of events";
  EnuMin = 0;
  EnuMax = 6.0;
  fIsDiag = true;
  fIsRawEvents = true;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/ANL/CC1pip_on_p/ANL_CC1ppip_phi.csv");
  this->SetupDefaultHist();

  // set Poisson errors on fDataHist (scanned does not have this)
  // Simple counting experiment here
  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i+1, sqrt(fDataHist->GetBinContent(i+1)));
  }

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  TRandom3 rand;

  this->fScaleFactor = this->fEventHist->Integral("width")/(fNEvents+0.)*16./8.;
};


void ANL_CC1ppip_Evt_1Dphi_nu::FillEventVariables(FitEvent *event) {
  
  // set up the 4-vectors from NEUT
  TLorentzVector Pnu = event->PartInfo(0)->fP;
  TLorentzVector Pp;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  // Loop over the particle stack to find relevant particles 
  // start at 2 because 0=nu, 1=nucleon, by NEUT default
  for (UInt_t j =  2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue; //move on if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211) {
      Ppip = event->PartInfo(j)->fP;
    } else if (PID == 2212) {
      Pp = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;
    }
  }
  
  // Get the hadronic mass
  double hadMass = FitUtils::MpPi(Pp, Ppip);
  // Need to boost pion and muon into resonance rest-frame to get phi (e.g. see F. Sanchez arxiv 1511.00501v2)
  //
  // Get the resonance 4-vector
  TLorentzVector Pres = Ppip + Pp;
  // Boost the outgoing and incoming particles into the resonance frame
  Pnu.Boost(Pres.BoostVector());
  Pmu.Boost(-Pres.BoostVector());
  Ppip.Boost(-Pres.BoostVector());

  // Get the vectors from the 4-vector
  TVector3 PmuVect = Pmu.Vect();
  TVector3 PnuVect = Pnu.Vect();
  TVector3 PresVect = Pres.Vect();
  TVector3 PpipVect = Ppip.Vect();

  // Define the z-direction
  TVector3 zVect = (PnuVect-PmuVect);
  zVect *= 1/double(zVect.Mag());
  // Define y direction as being z (resonance direction) x pmu*
  TVector3 yVect = zVect.Cross(PmuVect);
  // Normalise yVector
  yVect *= 1/double(yVect.Mag());
  // define x direction as being y X z
  TVector3 xVect = yVect.Cross(zVect);
  // Normalise zVector
  xVect *= 1/double(xVect.Mag());

  // Project pion onto z axis
  TVector3 PpipVectZ = zVect * PpipVect.Dot(zVect);
  // Then subtract this vector off the pion vector
  TVector3 PpipVectPlane = PpipVect - PpipVectZ;

  // Then finally construct phi as the angle between pion projection and x axis
  double phi;
  
  // ANL has a M(pi, p) < 1.4 GeV cut imposed
  if (hadMass < 1400) {
    if (PpipVectPlane.Y() > 0) {
      phi = (180./M_PI)*PpipVectPlane.Angle(xVect);
    } else if (PpipVectPlane.Y() < 0) {
      phi = (180./M_PI)*(2*M_PI-PpipVectPlane.Angle(xVect));
    } else if (PpipVectPlane.Y() == 0) {
      double randNo = rand.Rndm();
      if (randNo > 0.5) {
        phi = (180./M_PI)*PpipVectPlane.Angle(xVect);
      } else {
        phi = (180./M_PI)*(2*M_PI-PpipVectPlane.Angle(xVect));
      }
    }
  } else {
    phi = -999;
  }

  fXVar = phi;

  return;
};

bool ANL_CC1ppip_Evt_1Dphi_nu::isSignal(FitEvent *event) {

  if ((event->PartInfo(0))->fPID != 14) return false;

  if (((event->PartInfo(0))->fP.E() < this->EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > this->EnuMax*1000.)) return false; 

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false; 

  int pipCnt = 0;
  int lepCnt = 0;
  int protonCnt = 0;

  for (UInt_t j =  2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 13) {
      lepCnt++;
    } else if (PID == 211) {
      pipCnt++;
    } else if (PID == 2212) {
      protonCnt++;
    } else {
      return false; // require only three prong events! (allow photons?)
    }
  }

  // don't think there's away of implementing spectator proton cuts in NEUT?
  // 100 MeV or larger protons

  if (pipCnt != 1) return false;
  if (lepCnt != 1) return false;
  if (protonCnt != 1) return false;

  return true;
}

/*
void ANL_CC1ppip_Evt_1Dphi_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void ANL_CC1ppip_Evt_1Dphi_nu::ScaleEvents() {
  
  PlotUtils::FluxUnfoldedScaling(fMCHist, fFluxHist);
  PlotUtils::FluxUnfoldedScaling(fMCFine, fFluxHist);

  fMCHist->Scale(fScaleFactor);
  fMCFine->Scale(fScaleFactor);

  return;
}
*/
