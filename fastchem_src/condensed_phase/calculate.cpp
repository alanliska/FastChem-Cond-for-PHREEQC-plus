/*
* This file is part of the FastChem code (https://github.com/exoclime/fastchem).
* Copyright (C) 2022 Daniel Kitzmann, Joachim Stock
*
* FastChem is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* FastChem is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You find a copy of the GNU General Public License in the main
* FastChem directory under <license.md>. If not, see
* <http://www.gnu.org/licenses/>.
*/


#include <string>
#include <vector>
#include <iostream>
#include <cmath>

#include "condensed_phase.h"

#include "../fastchem_constants.h"
#include "../species_struct.h"
#include "solver.h"
#include "../../_ext/Eigen/Dense"


namespace fastchem {


//This is the main FastChem iteration for the gas phase
template <class double_type>
bool CondensedPhase<double_type>::calculate(
  const double temperature,
  const double density,
  const double total_element_density,
  std::vector<Molecule<double_type>>& molecules,
  unsigned int& nb_iterations)
{
  std::vector<Condensate<double_type>*> condensates_act;
  std::vector<Element<double_type>*> elements_cond;

  double_type tau = 1e-25;
  double_type log_tau = std::log10(tau);


  selectActiveCondensates(condensates_act, elements_cond);


  std::vector<unsigned int> condensates_jac;

  for (size_t i=0; i<condensates_act.size(); ++i)
    condensates_jac.push_back(i);


  std::vector<unsigned int> condensates_rem;


  std::vector<double_type> elem_densities_old(elements_cond.size(), 0.0);
  std::vector<double_type> elem_densities_new(elements_cond.size(), 0.0);

  for (size_t i=0; i<elements_cond.size(); ++i)
    elem_densities_old[i] = elements_cond[i]->number_density;

  std::vector<double_type> cond_densities_old(condensates_act.size(), tau);
  std::vector<double_type> cond_densities_new(condensates_act.size(), 0.0);

  std::vector<double_type> activity_corr_old(condensates_act.size(), 1.0);
  std::vector<double_type> activity_corr_new(condensates_act.size(), 0.0);


  for (unsigned int it=0; it<1000; ++it)
  {
    Eigen::MatrixXdt<double_type> jacobian = solver.assembleJacobian(
      condensates_act,
      activity_corr_old,
      cond_densities_old,
      condensates_jac,
      condensates_rem,
      elements_cond,
      molecules);

    Eigen::VectorXdt<double_type> rhs = solver.assembleRightHandSide(
      condensates_act,
      condensates_jac,
      condensates_rem,
      activity_corr_old,
      cond_densities_old,
      elements_cond,
      molecules,
      total_element_density,
      log_tau);

    std::cout << jacobian << "\n";
    std::cout << rhs << "\n";

    std::vector<double_type> result = solver.solveSystem(jacobian, rhs);
  
    std::cout << "\n";

    for (auto & i : result)
      std::cout << i << "\n";

    correctValues(
      result,
      condensates_act,
      condensates_jac,
      condensates_rem,
      activity_corr_old,
      activity_corr_new,
      cond_densities_old,
      cond_densities_new,
      elements_cond,
      elem_densities_old,
      elem_densities_new,
      log_tau,
      10.0);

    for (size_t i=0; i<elements_cond.size(); ++i)
      elements_cond[i]->number_density = elem_densities_new[i];

    for (auto & i : condensates_act)  i->calcActivity(temperature, elements);

    for (auto & i : molecules)  i.calcNumberDensity(elements);

    for (size_t i=0; i<condensates_act.size(); ++i)
      std::cout << i << "\t" << cond_densities_old[i] << "\t" << cond_densities_new[i] << "\t" << activity_corr_old[i] << "\t" << activity_corr_new[i] << "\t" << condensates_act[i]->log_activity << "\n";

    elem_densities_old = elem_densities_new;
    cond_densities_old = cond_densities_new;
    activity_corr_old = activity_corr_new;
  }


  

  

  exit(0);
  
  nb_iterations = 1;

  return true;
}



template <class double_type>
void CondensedPhase<double_type>::correctValues(
  const std::vector<double_type>& result,
  const std::vector<Condensate<double_type>*>& condensates,
  const std::vector<unsigned int>& condensates_jac,
  const std::vector<unsigned int>& condensates_rem,
  const std::vector<double_type>& activity_corr_old,
  std::vector<double_type>& activity_corr_new,
  const std::vector<double_type>& cond_number_dens_old,
  std::vector<double_type>& cond_number_dens_new,
  const std::vector<Element<double_type>*>& elements,
  const std::vector<double_type>& elem_number_dens_old,
  std::vector<double_type>& elem_number_dens_new,
  const double_type ln_tau,
  const double max_change)
{
  std::vector<double_type> delta_n(condensates.size(), 0);

  for (size_t i=0; i<condensates_jac.size(); ++i)
    delta_n[condensates_jac[i]] = result[i];


  const size_t nb_cond_jac = condensates_jac.size();

  for (size_t i=0; i<condensates_rem.size(); ++i)
  {
    const unsigned int index = condensates_rem[i];

    for (size_t j=0; j<elements.size(); ++j)
      delta_n[index] += condensates[index]->stoichiometric_vector[elements[j]->index] * result[j+nb_cond_jac];

    delta_n[index] /= activity_corr_old[index];

    delta_n[index] += condensates[index]->log_activity / activity_corr_old[index] + ln_tau - std::log(activity_corr_old[index]) - std::log(cond_number_dens_old[index]) + 1;
  }


  for (size_t i=0; i<condensates.size(); ++i)
  {
    if (delta_n[i] > max_change) delta_n[i] = max_change;
    if (delta_n[i] < -max_change) delta_n[i] = -max_change;

    cond_number_dens_new[i] = cond_number_dens_old[i] * std::exp(delta_n[i]);

    double_type delta_lambda = ln_tau - std::log(activity_corr_old[i]) - std::log(cond_number_dens_old[i]) - delta_n[i];
    activity_corr_new[i] = activity_corr_old[i] * std::exp(delta_lambda);
  }


  for (size_t i=0; i<elements.size(); ++i)
  { 
    double_type delta_n = result[i + nb_cond_jac];

    if (delta_n > max_change) delta_n = max_change;
    if (delta_n < -max_change) delta_n = -max_change;

    elem_number_dens_new[i] = elem_number_dens_old[i] * std::exp(delta_n);
  }

}



template class CondensedPhase<double>;
template class CondensedPhase<long double>;
}