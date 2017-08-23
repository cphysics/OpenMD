/*
 * Copyright (c) 2005 The University of Notre Dame. All Rights Reserved.
 *
 * The University of Notre Dame grants you ("Licensee") a
 * non-exclusive, royalty free, license to use, modify and
 * redistribute this software in source and binary code form, provided
 * that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 * This software is provided "AS IS," without a warranty of any
 * kind. All express or implied conditions, representations and
 * warranties, including any implied warranty of merchantability,
 * fitness for a particular purpose or non-infringement, are hereby
 * excluded.  The University of Notre Dame and its licensors shall not
 * be liable for any damages suffered by licensee as a result of
 * using, modifying or distributing the software or its
 * derivatives. In no event will the University of Notre Dame or its
 * licensors be liable for any lost revenue, profit or data, or for
 * direct, indirect, special, consequential, incidental or punitive
 * damages, however caused and regardless of the theory of liability,
 * arising out of the use of or inability to use software, even if the
 * University of Notre Dame has been advised of the possibility of
 * such damages.
 *
 * SUPPORT OPEN SCIENCE!  If you use OpenMD or its source code in your
 * research, please cite the appropriate papers when you publish your
 * work.  Good starting points are:
 *                                                                      
 * [1]  Meineke, et al., J. Comp. Chem. 26, 252-271 (2005).             
 * [2]  Fennell & Gezelter, J. Chem. Phys. 124, 234104 (2006).          
 * [3]  Sun, Lin & Gezelter, J. Chem. Phys. 128, 234107 (2008).          
 * [4] Kuang & Gezelter,  J. Chem. Phys. 133, 164101 (2010).
 * [4] , Stocker & Gezelter, J. Chem. Theory Comput. 7, 834 (2011).
 */

/* Calculates Rho(Y,Z) for density profile of liquid slab. */

#include <algorithm>
#include <fstream>
#include "applications/staticProps/RhoYZ.hpp"
#include "utils/simError.h"
#include "io/DumpReader.hpp"
#include "primitives/Molecule.hpp"

namespace OpenMD {
  
  RhoYZ::RhoYZ(SimInfo* info, const std::string& filename, 
               const std::string& sele, int nybins, int nzbins)
    : StaticAnalyser(info, filename, nzbins), selectionScript_(sele), 
      evaluator_(info), seleMan_(info), nYBins_(nybins) {
    
    evaluator_.loadScriptString(sele);
    if (!evaluator_.isDynamic()) {
      seleMan_.setSelectionSet(evaluator_.evaluate());
    }       
    
    // fixed number of bins

    sliceSDLists_.resize(nYBins_);
    density_.resize(nYBins_);
    for (unsigned int i = 0 ; i < nYBins_; ++i) {
      sliceSDLists_[i].resize(nBins_);
      density_[i].resize(nBins_);
    }

    setOutputName(getPrefix(filename) + ".RhoYZ");
  }

  void RhoYZ::process() {
    StuntDouble* sd;
    int ii;

    bool usePeriodicBoundaryConditions_ = 
      info_->getSimParams()->getUsePeriodicBoundaryConditions();

    DumpReader reader(info_, dumpFilename_);    
    int nFrames = reader.getNFrames();
    nProcessed_ = nFrames/step_;

    for (int istep = 0; istep < nFrames; istep += step_) {
      reader.readFrame(istep);
      currentSnapshot_ = info_->getSnapshotManager()->getCurrentSnapshot();

      for (unsigned int i = 0; i < nYBins_; i++) {
        for (unsigned int j = 0; j < nBins_; j++) {          
          sliceSDLists_[i][j].clear();
        }
      }

      RealType sliceVolume = currentSnapshot_->getVolume() /(nYBins_ * nBins_);
      Mat3x3d hmat = currentSnapshot_->getHmat();
      
      yBox_.push_back(hmat(1,1));
      zBox_.push_back(hmat(2,2));

      RealType halfBoxY_ = hmat(1,1) / 2.0;      
      RealType halfBoxZ_ = hmat(2,2) / 2.0;      

      if (evaluator_.isDynamic()) {
        seleMan_.setSelectionSet(evaluator_.evaluate());
      }
      
      //wrap the stuntdoubles into a cell      
      for (sd = seleMan_.beginSelected(ii); sd != NULL; 
	   sd = seleMan_.nextSelected(ii)) {
        Vector3d pos = sd->getPos();
        if (usePeriodicBoundaryConditions_)
          currentSnapshot_->wrapVector(pos);
        sd->setPos(pos);
      }
      
      //determine which atom belongs to which slice
      for (sd = seleMan_.beginSelected(ii); sd != NULL; 
	   sd = seleMan_.nextSelected(ii)) {
        Vector3d pos = sd->getPos();
        // shift molecules by half a box to have bins start at 0
        int binNoY = int(nYBins_ * (halfBoxY_ + pos.y()) / hmat(1,1));
        int binNoZ = int(nBins_  * (halfBoxZ_ + pos.z()) / hmat(2,2));
        sliceSDLists_[binNoY][binNoZ].push_back(sd);
      }

      //loop over the slices to calculate the densities
      for (unsigned int i = 0; i < nYBins_; i++) {
        for (unsigned int j = 0; j < nBins_; j++) {

          RealType totalMass = 0;
          for (unsigned int k = 0; k < sliceSDLists_[i][j].size(); ++k) {
            totalMass += sliceSDLists_[i][j][k]->getMass();
          }
          density_[i][j] += totalMass/sliceVolume;
        }
      }
    }
      
    writeDensity();

  }
  
  
  
  void RhoYZ::writeDensity() {

    std::vector<RealType>::iterator j;
    std::ofstream rdfStream(outputFilename_.c_str());

    if (rdfStream.is_open()) {
      rdfStream << "#RhoYZ\n";
      rdfStream << "#nFrames:\t" << nProcessed_ << "\n";
      rdfStream << "#selection: (" << selectionScript_ << ")\n";
      rdfStream << "#density (Y,Z)\n";
      for (unsigned int i = 0; i < density_.size(); ++i) {
        for (unsigned int j = 0; j < density_[i].size(); ++j) {          
          rdfStream << Constants::densityConvert * density_[i][j] / nProcessed_;
          rdfStream << "\t";
        }
        rdfStream << "\n";
      }
      
    } else {
      
      sprintf(painCave.errMsg, "RhoYZ: unable to open %s\n", 
	      outputFilename_.c_str());
      painCave.isFatal = 1;
      simError();  
    }
    
    rdfStream.close();
  }
  
}

