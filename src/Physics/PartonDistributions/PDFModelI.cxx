//____________________________________________________________________________
/*
 Copyright (c) 2003-2018, The GENIE Collaboration
 For the full text of the license visit http://copyright.genie-mc.org
 or see $GENIE/LICENSE

 Author: Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
         University of Liverpool & STFC Rutherford Appleton Lab 

 For the class documentation see the corresponding header file.

 Important revisions after version 2.0.0 :

*/
//____________________________________________________________________________

#include "Physics/PartonDistributions/PDFModelI.h"

using namespace genie;

//____________________________________________________________________________
PDFModelI::PDFModelI() :
Algorithm()
{

}
//____________________________________________________________________________
PDFModelI::PDFModelI(string name) :
Algorithm(name)
{

}
//____________________________________________________________________________
PDFModelI::PDFModelI(string name, string config) :
Algorithm(name, config)
{

}
//____________________________________________________________________________
PDFModelI::~PDFModelI() 
{ 

}
//____________________________________________________________________________



