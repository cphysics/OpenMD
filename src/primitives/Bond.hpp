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
 * [4]  Kuang & Gezelter,  J. Chem. Phys. 133, 164101 (2010).
 * [5]  Vardeman, Stocker & Gezelter, J. Chem. Theory Comput. 7, 834 (2011).
 */
 
/**
 * @file Bond.hpp
 * @author    tlin
 * @date  11/01/2004
 * @version 1.0
 */ 


#ifndef PRIMITIVES_BOND_HPP
#define PRIMITIVES_BOND_HPP

#include "primitives/ShortRangeInteraction.hpp"
#include "primitives/Atom.hpp"
#include "types/BondType.hpp"

namespace OpenMD {
  
  class Bond : public ShortRangeInteraction {
  public:
    using ShortRangeInteraction::getValue;
    using ShortRangeInteraction::getPrevValue;
    Bond(Atom* atom1, Atom* atom2, BondType* bt) : ShortRangeInteraction(),
                                                   bondType_(bt) {
      atoms_.resize(2);
      atoms_[0] = atom1;
      atoms_[1] = atom2;
    }
    virtual ~Bond() {}
    void calcForce(bool doParticlePot) {
      RealType len;
      RealType dvdr;
      Vector3d r12;
      Vector3d force;
      
      r12 = atoms_[1]->getPos() - atoms_[0]->getPos();
      snapshotMan_->getCurrentSnapshot()->wrapVector(r12);
      len = r12.length();            
      bondType_->calcForce(len,  potential_, dvdr);
      
      force = r12 * (-dvdr / len);

      atoms_[0]->addFrc(-force);
      atoms_[1]->addFrc(force);
      if (doParticlePot) {
        atoms_[0]->addParticlePot(potential_);
        atoms_[1]->addParticlePot(potential_);
      }
    }
    
    RealType getValue(int snap) {
      Vector3d r12 = atoms_[1]->getPos(snap) - atoms_[0]->getPos(snap);
      snapshotMan_->getSnapshot(snap)->wrapVector(r12);
      return r12.length();            
    }

    RealType getPotential() {
      return potential_;
    }
    
    Atom* getAtomA() {
      return atoms_[0];
    }
    
    Atom* getAtomB() {
      return atoms_[1];
    }
    
    BondType* getBondType() {
      return bondType_;
    }

    virtual std::string getName() { return name_;}        
    /** Sets the name of this bond for selections */
    virtual void setName(const std::string& name) { name_ = name; }

    void accept(BaseVisitor* v) {
      v->visit(this);
    }    
    
  private:
    RealType potential_;
    BondType* bondType_; /**< bond type */
    std::string name_;

  };    
} //end namespace OpenMD
#endif //PRIMITIVES_BOND_HPP
