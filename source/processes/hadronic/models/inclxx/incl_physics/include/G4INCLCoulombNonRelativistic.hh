//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// INCL++ intra-nuclear cascade model
// Pekka Kaitaniemi, CEA and Helsinki Institute of Physics
// Davide Mancusi, CEA
// Alain Boudard, CEA
// Sylvie Leray, CEA
// Joseph Cugnon, University of Liege
//
#define INCLXX_IN_GEANT4_MODE 1

#include "globals.hh"

/** \file G4INCLCoulombNonRelativistic.hh
 * \brief Class for non-relativistic Coulomb distortion.
 *
 * \date 14 February 2011
 * \author Davide Mancusi
 */

#ifndef G4INCLCOULOMBNONRELATIVISTIC_HH_
#define G4INCLCOULOMBNONRELATIVISTIC_HH_

#include "G4INCLParticle.hh"
#include "G4INCLNucleus.hh"
#include "G4INCLICoulomb.hh"
#include "G4INCLCoulombNone.hh"
#include "G4INCLGlobals.hh"

namespace G4INCL {

  class CoulombNonRelativistic : public ICoulomb {
    public:
      CoulombNonRelativistic() {}
      virtual ~CoulombNonRelativistic() {}

      /** \brief Modify the momentum of the particle and position it on the
       *         surface of the nucleus.
       *
       * This method performs non-relativistic distortion.
       *
       * \param p incoming particle
       * \param n distorting nucleus
       **/
      ParticleEntryAvatar *bringToSurface(Particle * const p, Nucleus * const n) const;

      /** \brief Modify the momentum of the incoming cluster and position it on
       *         the surface of the nucleus.
       *
       * This method performs non-relativistic distortion. The momenta of the
       * particles that compose the cluster are also distorted.
       *
       * \param c incoming cluster
       * \param n distorting nucleus
       **/
      IAvatarList bringToSurface(Cluster * const c, Nucleus * const n) const;

      /** \brief Modify the momenta of the outgoing particles.
       *
       * This method performs non-relativistic distortion.
       *
       * \param pL list of outgoing particles
       * \param n distorting nucleus
       */
      void distortOut(ParticleList const &pL, Nucleus const * const n) const;

      /** \brief Return the maximum impact parameter for Coulomb-distorted
       *         trajectories. **/
      G4double maxImpactParameter(ParticleSpecies const &p, const G4double kinE, Nucleus const *
          const n) const;

    private:
      /// \brief Return the maximum impact parameter for Coulomb-distorted trajectories.
      G4double maxImpactParameterParticle(ParticleSpecies const &p, const G4double kinE, Nucleus const *
          const n) const;

      /// \brief Return the minimum distance of approach in a head-on collision (b=0).
      G4double minimumDistance(ParticleSpecies const &p, const G4double kineticEnergy, Nucleus const * const n) const {
        const G4double particleMass = ParticleTable::getTableSpeciesMass(p);
        const G4double nucleusMass = n->getTableMass();
        const G4double reducedMass = particleMass*nucleusMass/(particleMass+nucleusMass);
        return PhysicalConstants::eSquared * p.theZ * n->getZ() * particleMass
          / (kineticEnergy * reducedMass);
      }

      /// \brief Return the minimum distance of approach in a head-on collision (b=0).
      G4double minimumDistance(Particle const * const p, Nucleus const * const n) const {
        return minimumDistance(p->getSpecies(), p->getKineticEnergy(), n);
      }

      /** \brief Perform Coulomb deviation
       *
       * Modifies the entrance angle of the particle and its impact parameter.
       * Can be applied to Particles and Clusters.
       *
       * The trajectory for an asymptotic impact parameter \f$b\f$ is
       * parametrised as follows:
       * \f[
       * r(\theta) = \frac{(1-e^2)r_0/2}{1-e \sin(\theta-\theta_R/2)},
       * \f]
       * here \f$e\f$ is the hyperbola eccentricity:
       * \f[
       * e = \sqrt{1+4b^2/r_0^2};
       * \f]
       * \f$\theta_R\f$ is the Rutherford scattering angle:
       * \f[
       * \theta_R = \pi - 2\arctan\left(\frac{2b}{r_0}\right)
       * \f]
       * \f$\theta\f$ ranges from \f$\pi\f$ (initial state) to \f$\theta_R\f$
       * (scattered particle) and \f$r_0\f$ is the minimum distance of approach
       * in a head-on collision (see the minimumDistance() method).
       *
       * \param p pointer to the Particle
       * \param n pointer to the Nucleus
       * \return false if below the barrier
       */
      G4bool coulombDeviation(Particle * const p, Nucleus const * const n) const;

      /// \brief Internal CoulombNone slave to generate the avatars
      CoulombNone theCoulombNoneSlave;
  };
}

#endif /* G4INCLCOULOMBNONRELATIVISTIC_HH_ */
