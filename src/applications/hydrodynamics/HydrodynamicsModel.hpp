/*
 * Copyright (c) 2005 The University of Notre Dame. All Rights Reserved.
 *
 * The University of Notre Dame grants you ("Licensee") a
 * non-exclusive, royalty free, license to use, modify and
 * redistribute this software in source and binary code form, provided
 * that the following conditions are met:
 *
 * 1. Acknowledgement of the program authors must be made in any
 *    publication of scientific results based in part on use of the
 *    program.  An acceptable form of acknowledgement is citation of
 *    the article in which the program was described (Matthew
 *    A. Meineke, Charles F. Vardeman II, Teng Lin, Christopher
 *    J. Fennell and J. Daniel Gezelter, "OOPSE: An Object-Oriented
 *    Parallel Simulation Engine for Molecular Dynamics,"
 *    J. Comput. Chem. 26, pp. 252-271 (2005))
 *
 * 2. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 3. Redistributions in binary form must reproduce the above copyright
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
 */
#ifndef APPLICATION_HYDRODYNAMICS_HYDRODYNAMICSMODEL_HPP
#define APPLICATION_HYDRODYNAMICS_HYDRODYNAMICSMODEL_HPP
#include <vector>

#include "math/Vector3.hpp"
#include "math/SquareMatrix3.hpp"
#include "math/DynamicRectMatrix.hpp"
#include "primitives/Molecule.hpp"
#include "hydrodynamics/HydroProp.hpp"
#include "utils/OOPSEConstant.hpp"

namespace oopse {
  
  struct BeadParam {
    std::string atomName;
    Vector3d pos;
    RealType radius;
  };
  
  class Shape;
  class Sphere;
  class Ellipsoid;
  class CompositeShape;
  
  class HydrodynamicsModel {
  public:
    HydrodynamicsModel(StuntDouble* sd, SimInfo* info) : sd_(sd), info_(info) {}
    virtual ~HydrodynamicsModel() {}

    virtual bool calcHydroProps(Shape* shape, RealType viscosity, RealType temperature);
    
    virtual void init() {};
    virtual void writeBeads(std::ostream& os) = 0;
    void writeHydroProps(std::ostream& os);
    HydroProp* getHydroPropsAtCR() {return cr_;}
    HydroProp* getHydroPropsAtCD() {return cd_;}
    
    void setCR(HydroProp* cr) {cr_ = cr;}
    void setCD(HydroProp* cd) {cd_ = cd;}
    std::string getStuntDoubleName() { return sd_->getType();}
  protected:
    StuntDouble* sd_;
    SimInfo* info_;
  private:
    HydroProp* cr_;
    HydroProp* cd_;
    std::vector<BeadParam> beads_;
  };
  
}

#endif
