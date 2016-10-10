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

#ifndef JOINTMEASUREMENT_1D_H_SEEN
#define JOINTMEASUREMENT_1D_H_SEEN
/*!
 *  \addtogroup FitBase
 *  @{
 */


#include <stdlib.h>
#include <numeric>
#include <math.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <deque>
#include <list>

// ROOT includes
#include <TROOT.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TArrayF.h>
#include <TGraph.h>
#include <TCut.h>
#include <TGraphErrors.h>
#include <TMatrixDSym.h>
#include <TDecompSVD.h>
#include <TDecompChol.h>
#include <TSystem.h>
#include <TCanvas.h>
#include "Measurement1D.h"

// External data fit includes
#include "MeasurementBase.h"
#include "FitEvent.h"
#include "FitUtils.h"
#include "StatUtils.h"
#include "PlotUtils.h"
#include "FitParameters.h"

#include "InputHandler.h"

/// Joint Measurement 1D Class
///
/// Base class used to setup measurements that require multiple distributions to then be merged.
/// The "fSubChain" object is used to keep track of each of the individual experiments which are then
/// automatically reconfigured, written, etc.

//********************************************************************
//! Base class to setup measurements that require several sub measurements
class JointMeas1D : public Measurement1D {
//********************************************************************

 public:

  /*
    Constructor/Deconstuctor
  */

  //! Default Constructor
  JointMeas1D();

  //! Default Virtual Destructor
  virtual ~JointMeas1D();

  //! Initialise the samples
  void Init();

  /*
    Worker Node Functions
    - Input handler does the long reconfigures so gives option for cluster submission
    - on a sample by sample basis
  */


  /*
    Setup Functions
  */
  /// Setup the measurement and weight engines, parse the input files and setup sub measurements.
  virtual void SetupMeasurement(std::string input, std::string type, FitWeight *rw, std::string fkdt);

  /*
    XSec Functions
  */
  /// Return total integrated flux. Will integrate flux of all sub samples if required.
  virtual double TotalIntegratedFlux(std::string intOpt="width",double low=-9999.9, double high=-9999.9);

  /*
    Reconfigure Functions
  */

  /// Call reconfigure on every sub sample
  virtual void Reconfigure();
  virtual void ReconfigureFast();

  /// Stitch the sub sample plots together to make a final fMCHist after reconfigure has been called
  virtual void MakePlots();

  /*
    Access Functions
  */
  virtual std::vector<TH1*> GetMCList();
  virtual std::vector<TH1*> GetDataList();
  virtual std::vector<TH1*> GetFineList();
  virtual std::vector<TH1*> GetMaskList();
  virtual std::vector<TH1*> GetFluxList();
  virtual std::vector<TH1*> GetEventRateList();
  virtual std::vector<TH1*> GetXSecList();

  //! Return a flux integrated across all sub samples
  virtual TH1D* GetCombinedFlux();

  //! Return an event rate integrated across all sub samples
  virtual TH1D* GetCombinedEventRate();

  /*
    Write Functions
  */

  //! Write the current status of the plots to the current directory
  virtual void Write(std::string drawOpt);

  std::vector<MeasurementBase*> fSubChain; //!< Vector of experimental classes that are the sub measurements
  std::vector<std::string>      fSubInFiles; //!< vector of input files for each of the sub measurements.

protected:

  bool fIsRatio; //!< Flag: is this sample a hist1/hist2 ratio sample
  bool fIsSummed; //!< Flag: is this sample a combination hist1 + hist2
  bool fSaveSubMeas; //!< Flag: Save each of the histograms from the sub samples as well as this joint samples plots

};

/*! @} */
#endif
