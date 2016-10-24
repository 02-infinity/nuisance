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

#include "JointMeas1D.h"

  /*
    Constructor/Deconstuctor
  */

//********************************************************************
JointMeas1D::JointMeas1D(){
//********************************************************************

  // Reset list
  for (std::vector<MeasurementBase*>::const_iterator iter = fSubChain.begin(); iter != fSubChain.end(); iter++){
    MeasurementBase* exp = *iter;
    if (exp) delete exp;

  }

  // Reset Variables
  this->Init();

}


//********************************************************************
JointMeas1D::~JointMeas1D(){
//********************************************************************

  // Delete sub experiments
  for (std::vector<MeasurementBase*>::const_iterator iter = fSubChain.begin(); iter != fSubChain.end(); iter++){
    MeasurementBase* exp = *iter;
    if (exp) delete exp;
  }

}


//********************************************************************
void JointMeas1D::Init(){
//********************************************************************

  // Flags for Joint Measurements
  fIsRatio = false;
  fIsSummed = false;
  fSaveSubMeas = false;

  // Clear Input Files
  fSubInFiles.clear();

  return;
}



/*
  Setup Functions
*/
//********************************************************************
void JointMeas1D::SetupMeasurement(std::string input, std::string type, FitWeight *rw, std::string fkdt){
//********************************************************************

  // For joint samples, input files are given as a semi-colon seperated list.
  // Parse this list and save it for later, and set up the types etc.

  fSubInFiles.clear();
  std::istringstream stream(input);
  std::string temp_string;

  while (std::getline(stream >> std::ws, temp_string, ';')) {
    fSubInFiles.push_back(temp_string);
  }

  // Set Engine and Fake Data
  fRW = rw;
  fakeDataFile = fkdt;

  // Set Fit Options
  SetFitOptions(type);

  return;

}

  /*
    XSec Functions
  */

//********************************************************************
double JointMeas1D::TotalIntegratedFlux(std::string intOpt,double low, double high){
//********************************************************************

  double totalflux = 0.0;

  // Destroy the job for sub samples
  for (std::vector<MeasurementBase*>::const_iterator expIter = fSubChain.begin(); expIter != fSubChain.end(); expIter++){
    MeasurementBase* exp = *expIter;
    double expflux = exp->TotalIntegratedFlux(intOpt,low,high);

    // Fill flux options
    if (fIsRatio){
      totalflux = expflux;
      break;
    }
    if (fIsSummed){
      totalflux += expflux;
    }
  }

  return totalflux;
}

  /*
    Reconfigure Functions
  */

//********************************************************************
void JointMeas1D::Reconfigure(){
//********************************************************************

  // This will just call reconfigure explicitly and apply all the standard scalings too each sub sample.
  // If you just want to fill event rates into plots make a Reconfigure in the top level sample, you will
  // need to loop over each sample explicitly and cast it to an InputHandler before calling ReconfigureAllEvents.
  for (std::vector<MeasurementBase*>::const_iterator expIter = fSubChain.begin(); expIter != fSubChain.end(); expIter++){
    MeasurementBase* exp = *expIter;
    exp->Reconfigure();
  }

  // Joint function called by top level class
  MakePlots();

  // Do Final Normalisation
  ApplyNormScale( fRW->GetSampleNorm( this->fName ) );

  return;
}


//********************************************************************
void JointMeas1D::ReconfigureFast(){
  //********************************************************************
  // This will just call reconfigure explicitly and apply all the standard scalings too each sub sample.
  // If you just want to fill event rates into plots make a Reconfigure in the top level sample, you will
  // need to loop over each sample explicitly and cast it to an InputHandler before calling ReconfigureAllEvents.
  for (std::vector<MeasurementBase*>::const_iterator expIter = fSubChain.begin(); expIter != fSubChain.end(); expIter++){
    MeasurementBase* exp = *expIter;
    exp->ReconfigureFast();
  }

  // Joint function called by top level class
  MakePlots();

  // Do Final Normalisation
  ApplyNormScale( fRW->GetSampleNorm( this->fName ) );

  return;
}

//********************************************************************
void JointMeas1D::MakePlots(){
//********************************************************************

  // Reset the 1D histograms but not the subClasses
  Measurement1D::ResetAll();

  // If Summed
  if (fIsSummed){

    for (std::vector<MeasurementBase*>::const_iterator expIter = fSubChain.begin(); expIter != fSubChain.end(); expIter++){
      MeasurementBase* exp = static_cast<MeasurementBase*>(*expIter);

      this->fMCHist->Add( exp->GetMCList().at(0)   );
      this->fMCFine->Add( exp->GetFineList().at(0) );

    }

    return;
  }

  // If Ratio
  if (fIsRatio){

    int sample = 0;
    for (std::vector<MeasurementBase*>::const_iterator expIter = fSubChain.begin(); expIter != fSubChain.end(); expIter++){
      MeasurementBase* exp = *expIter;

      if (sample == 0){

	this->fMCHist->Add( exp->GetMCList().at(0)    );
	this->fMCFine->Add( exp->GetFineList().at(0)  );

      } else if (sample == 1){

	this->fMCHist->Divide( exp->GetMCList().at(0)   );
	this->fMCFine->Divide( exp->GetFineList().at(0) );

      } else break;

      sample++;
    }
    return;
  }

  return;
}


/*
  Access Functions
*/

//********************************************************************
std::vector<TH1*> JointMeas1D::GetMCList(){
//********************************************************************

  // Make Default Vector
  std::vector<TH1*> tempVect;
  tempVect.push_back( this-> fMCHist );

  // Return vector from all sub samples
  for (std::vector<MeasurementBase*>::const_iterator expIter = fSubChain.begin(); expIter != fSubChain.end(); expIter++){
    MeasurementBase* exp = *expIter;

    std::vector<TH1*> subTempVect = exp->GetMCList();

    for(UInt_t i = 0; i < subTempVect.size(); i++){
      tempVect.push_back(subTempVect.at(i));
    }
  }

  return tempVect;
}

//********************************************************************
std::vector<TH1*> JointMeas1D::GetDataList(){
//********************************************************************

  // Make Default Vector
  std::vector<TH1*> tempVect;
  tempVect.push_back( this-> fDataHist );

  // Return vector from all sub samples
  for (std::vector<MeasurementBase*>::const_iterator expIter = fSubChain.begin(); expIter != fSubChain.end(); expIter++){
    MeasurementBase* exp = *expIter;

    std::vector<TH1*> subTempVect = exp->GetDataList();

    for(UInt_t i = 0; i < subTempVect.size(); i++){
      tempVect.push_back(subTempVect.at(i));
    }
  }

  return tempVect;
}

//********************************************************************
std::vector<TH1*> JointMeas1D::GetFineList(){
//********************************************************************

  // Make Default Vector
  std::vector<TH1*> tempVect;
  tempVect.push_back( this-> fMCFine );

  // Return vector from all sub samples
  for (std::vector<MeasurementBase*>::const_iterator expIter = fSubChain.begin(); expIter != fSubChain.end(); expIter++){
    MeasurementBase* exp = *expIter;

    std::vector<TH1*> subTempVect = exp->GetFineList();

    for(UInt_t i = 0; i < subTempVect.size(); i++){
      tempVect.push_back(subTempVect.at(i));
    }
  }

  return tempVect;
}

//********************************************************************
std::vector<TH1*> JointMeas1D::GetMaskList(){
//********************************************************************

  // Make Default Vector
  std::vector<TH1*> tempVect;
  tempVect.push_back( this-> fMaskHist );

  // Return vector from all sub samples
  for (std::vector<MeasurementBase*>::const_iterator expIter = fSubChain.begin(); expIter != fSubChain.end(); expIter++){
    MeasurementBase* exp = *expIter;

    std::vector<TH1*> subTempVect = exp->GetMaskList();

    for(UInt_t i = 0; i < subTempVect.size(); i++){
      tempVect.push_back(subTempVect.at(i));
    }
  }

  return tempVect;
}

//********************************************************************
std::vector<TH1*> JointMeas1D::GetFluxList(){
//********************************************************************

  // Make Default Vector
  std::vector<TH1*> tempVect;
  tempVect.push_back( this-> fFluxHist );

  // Return vector from all sub samples
  for (std::vector<MeasurementBase*>::const_iterator expIter = fSubChain.begin(); expIter != fSubChain.end(); expIter++){
    MeasurementBase* exp = *expIter;

    std::vector<TH1*> subTempVect = exp->GetFluxList();

    for(UInt_t i = 0; i < subTempVect.size(); i++){
      tempVect.push_back(subTempVect.at(i));
    }
  }

  return tempVect;
}

//********************************************************************
std::vector<TH1*> JointMeas1D::GetEventRateList(){
//********************************************************************

  // Make Default Vector
  std::vector<TH1*> tempVect;
  tempVect.push_back( this-> fEventHist );

  // Return vector from all sub samples
  for (std::vector<MeasurementBase*>::const_iterator expIter = fSubChain.begin(); expIter != fSubChain.end(); expIter++){
    MeasurementBase* exp = *expIter;

    std::vector<TH1*> subTempVect = exp->GetEventRateList();

    for(UInt_t i = 0; i < subTempVect.size(); i++){
      tempVect.push_back(subTempVect.at(i));
    }
  }

  return tempVect;
}


//********************************************************************
std::vector<TH1*> JointMeas1D::GetXSecList(){
//********************************************************************

  // Make Default Vector
  std::vector<TH1*> tempVect;
  tempVect.push_back( this-> fXSecHist );

  // Return vector from all sub samples
  for (std::vector<MeasurementBase*>::const_iterator expIter = fSubChain.begin(); expIter != fSubChain.end(); expIter++){
    MeasurementBase* exp = *expIter;

    std::vector<TH1*> subTempVect = exp->GetXSecList();

    for(UInt_t i = 0; i < subTempVect.size(); i++){
      tempVect.push_back(subTempVect.at(i));
    }
  }

  return tempVect;
}


//********************************************************************
TH1D* JointMeas1D::GetCombinedFlux(){
//********************************************************************

  TH1D* newflux;
  int sample = 0;

  for (std::vector<MeasurementBase*>::const_iterator expIter = fSubChain.begin(); expIter != fSubChain.end(); expIter++){
    MeasurementBase* exp = *expIter;

    // Get flux from experiment
    std::vector<TH1*> fluxVect =  exp->GetFluxList();

    // Setup newflux
    if (sample == 0){
      newflux = (TH1D*) fluxVect.at(0);
      newflux->Reset();
    }

    // Add all fluxes
    for (UInt_t i = 0; i < fluxVect.size(); i++){
      newflux->Add( (TH1D*) fluxVect.at(i) );
      sample++;
    }
  }

  return newflux;
}

//********************************************************************
TH1D* JointMeas1D::GetCombinedEventRate(){
//********************************************************************

  TH1D* newflux;
  int sample = 0;

  for (std::vector<MeasurementBase*>::const_iterator expIter = fSubChain.begin(); expIter != fSubChain.end(); expIter++){
    MeasurementBase* exp = *expIter;

    // Get flux from experiment
    std::vector<TH1*> fluxVect =  exp->GetFluxList();

    // Setup newflux
    if (sample == 0){
      newflux =(TH1D*) fluxVect.at(0);
      newflux->Reset();
    }

    // Add all fluxes
    for (UInt_t i = 0; i < fluxVect.size(); i++){
      newflux->Add( fluxVect.at(i) );
      sample++;
    }
  }

  return newflux;
}


  /*
    Write Functions
  */

//********************************************************************
void JointMeas1D::Write(std::string drawOpt){
//********************************************************************

  // Write the top level class
  Measurement1D::Write(drawOpt);

  if (fSaveSubMeas){
    for (std::vector<MeasurementBase*>::const_iterator expIter = fSubChain.begin(); expIter != fSubChain.end(); expIter++){
      MeasurementBase* exp = *expIter;
      exp->Write(drawOpt);
    }
  }

  return;
};


