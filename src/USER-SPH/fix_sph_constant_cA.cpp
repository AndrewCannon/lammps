/* ----------------------------------------------------------------------
 LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
 http://lammps.sandia.gov, Sandia National Laboratories
 Steve Plimpton, sjplimp@sandia.gov

 Copyright (2003) Sandia Corporation.  Under the terms of Contract
 DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
 certain rights in this software.  This software is distributed under
 the GNU General Public License.

 See the README file in the top-level LAMMPS directory.
 ------------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include "fix_sph_constant_cA.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "atom.h"
#include "comm.h"
#include "force.h"
#include "neighbor.h"
#include "neigh_list.h"
#include "neigh_request.h"
#include "update.h"
#include "integrate.h"
#include "respa.h"
#include "memory.h"
#include "error.h"
#include "pair.h"
#include <cstdio>

using namespace LAMMPS_NS;
using namespace FixConst;

/* ---------------------------------------------------------------------- */

FixSPHConstantcA::FixSPHConstantcA(LAMMPS *lmp, int narg, char **arg) :
  Fix(lmp, narg, arg) {

  if ((atom->esph_flag != 1) || (atom->rho_flag != 1))
    error->all(FLERR,
  	       "fix sph/constant/cA command requires atom_style with both energy and density, e.g. meso");

  if (narg != 4)
    error->all(FLERR,"Illegal number of arguments for fix sph/constant/cA command");

  time_integrate = 0;

  // Required args
  int m = 3;
  constcA = atof(arg[m++]);
  //  printf("arg: %s, cA: %3.3f\n", arg[m++], constcA);
  //  printf(" cA: %3.3f\n",  constcA);

  // find the concentration property
  int fcA;
  int icA = atom->find_custom("cA", fcA);
  if (icA < 0)
    error->all(FLERR,
	       "Can't find property cA for fix sph/constant/cA");
  cA = atom->dvector[icA];

  // find the local concentration property
  int fdcA;
  int idcA = atom->find_custom("dcA", fdcA);
  if (idcA < 0)
    error->all(FLERR,
	       "Can't find property dcA for fix sph/constant/cA");
  dcA = atom->dvector[idcA];
}

/* ---------------------------------------------------------------------- */

int FixSPHConstantcA::setmask() {
  int mask = 0;
  mask |= INITIAL_INTEGRATE;
  return mask;
}

/* ---------------------------------------------------------------------- */

void FixSPHConstantcA::initial_integrate(int vflag) {
  int *mask = atom->mask;
  int nlocal = atom->nlocal;
  int i;

  double **x = atom->x;
  
  if (igroup == atom->firstgroup)
    nlocal = atom->nfirst;

  for (i = 0; i < nlocal; i++) {
    if (mask[i] & groupbit) {
      //      printf("y: %.6f, cA: %3.3f\n", x[i], constcA);
      cA[i] = constcA; // keep the concentration at the constant level
      dcA[i] = 0.0; // and the change is 0.0
    }
  }
}

/* ---------------------------------------------------------------------- */
