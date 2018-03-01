#ifndef __GEN_BKG_FROM_HISTO_CXX__
#define __GEN_BKG_FROM_HISTO_CXX__

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TVector3.h"

void run (TH2 *histo, Int_t evnts = 1000000, 
	  Double_t tot_cs = 1.E4, Double_t target_thickness = 0.519,
	  Double_t target_density = 11.35, Double_t target_molmass = 207.2,
	  Bool_t debug = kFALSE)
{
//   Double_t tot_cs = 3.042E4; // barn
//   Double_t target_thickness = 0.519; // g/cm**2
//   Double_t target_density = 11.35; // g/cm**3
//   Double_t target_molmass = 207.2; // g/mol

  //const Char_t *out_fname = "/d/land2/drossi/sn132_208pb_500mev_1gcm2.dat";
  const Char_t *out_fname = "abkg_out.dat";

  Double_t target_l = target_thickness/target_density; // cm
  const Double_t barn_to_cm2 = 1.E-24;
  Double_t target_N = target_density * TMath::Na() / target_molmass; // 1/cm**3
  Double_t part_mfp = 1./(target_N * tot_cs * barn_to_cm2); // cm
  Double_t tar_start_offset = -0.5 * target_l; // cm

  TRandom3 rand(0);

  TH1F *h_int = new TH1F("h_int", "Interaction position", 1000, 0, 1.1 * target_l);
  TH1F *h_dist = new TH1F("h_dist", "Interaction distance from previous", 1000, 0, target_l);
  TH1F *h_mult = new TH1F("h_mult", "Interaction multiplicity", 1000, 0, 1000);
  TH1F *h_mult_g = new TH1F("h_mult_g", "Interaction multiplicity; gamma vector", 1000, 0, 1000);

  TH2F *h_sim = new TH2F("h_sim", "Simulated events: E vs. theta", 1000, 0, TMath::Pi(), 1000, 0, 3000);

  ofstream outfile;
  outfile.open(out_fname);

  if (!outfile.is_open())
    {
      std::cerr << "<-ERROR-> Output file not open! Aborting..." << std::endl;
      return;
    }

  for (int i=0; i<evnts; i++)
    {
      if (i%1000==0)
	std::cout << "<-INFO-> Processed " << i << " events (" << 100.*i/evnts << " %)..." << std::endl;
	

//       Double_t theta, E;
//       histo->GetRandom2(theta, E);

//       Double_t phi = rand.Rndm() * 360.;

//       if (debug)
// 	std::cout << "\t<-DEBUG-> th/phi/E: " << theta << "\t" << phi << "\t" << E << std::endl;

      std::vector<Double_t> pos;
      std::vector<TVector3> gamma;
      //int last_ind = 0;
      Double_t tpos = 0., last_tpos = 0.; // position in target

      do
	{
	  tpos += -TMath::Log(rand.Rndm()) * part_mfp;

	  if (tpos < target_l)
	    {
	      pos.push_back(tpos);
	      h_int->Fill(tpos);
	      h_dist->Fill(tpos - last_tpos);
	      last_tpos = tpos; 

	      Double_t theta, E;
	      histo->GetRandom2(theta, E);
	      
	      Double_t phi = rand.Rndm() * 360.;
	      
	      if (debug)
		std::cout << "\t<-DEBUG-> th/phi/E: " << theta << "\t" << phi << "\t" << E << std::endl;

	      TVector3 v_gam;
	      v_gam.SetMagThetaPhi(E, theta * TMath::DegToRad(), phi * TMath::DegToRad());
	      gamma.push_back(v_gam);
	    }
	}
      while (tpos < target_l);
	
      
      
#if 0     
      if (debug)
	std::cout << "   <-DEBUG-> " << target_l/step_size << ";  ";

      for (int j=0; j<target_l/step_size; j++)
	{
	  Double_t pdf = target_N * tot_cs * barn_to_cm2 * step_size * (j - last_ind) * TMath::Exp(-target_N * tot_cs * barn_to_cm2 * (j - last_ind) * step_size);
	  
	  if (pdf > rand.Rndm())
	    {
	      h_int->Fill(j * step_size);
	      h_dist->Fill((j - last_ind) * step_size);
	      pos.push_back(j);
	      last_ind = j;

	      if (debug)
		std::cout << j << " ";
	    }
	}
      
      if (debug)
	std::cout << std::endl;
#endif

      h_mult->Fill(pos.size());
      h_mult_g->Fill(gamma.size());

      outfile << i << "\t" << gamma.size() << "\t0.\t0.\n";
      
	for (uint j=0; j<gamma.size(); j++)
	{
	  h_sim->Fill(gamma.at(j).Theta(), gamma.at(j).Mag());
	  
	  outfile << "1\t0\t22\t" 
		  << gamma.at(j).X() * 1.E-6 << "\t" 
		  << gamma.at(j).Y() * 1.E-6 << "\t" 
		  << gamma.at(j).Z() * 1.E-6 << "\t"
		  << "0.\t0.\t" << pos.at(j) + tar_start_offset << "\n";
	  
	}

    }

  outfile.close();

  return;
}

#endif //  __GEN_BKG_FROM_HISTO_CXX__
