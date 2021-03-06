/**
 * @file   CompareCorrs.C
 * @author Christian Holm Christensen <cholm@dalsgaard.hehi.nbi.dk>
 * @date   Fri Jan 28 23:01:59 2011
 * 
 * @brief  Utilities for comparing correction objects 
 * 
 *
 * @ingroup pwglf_forward_scripts_corr
 * 
 */
#ifndef __CINT__
#include <TCanvas.h>
#include <TFile.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TMath.h>
#include "AliForwardCorrectionManager.h"
#endif


//======================================================================
/**
 * A canvas 
 * 
 *
 * @ingroup pwglf_forward_scripts_corr
 */
struct Canvas 
{
  //____________________________________________________________________
  /** 
   * CTOR
   * 
   * @param name  Name 
   * @param title Title 
   * @param n1    N1, 
   * @param n2    N2 
   */  
  Canvas(const char* name, const char* title,
	 const char* n1,   const char* n2)
    : fName(name),
      fTitle(title),
      fN1(n1), 
      fN2(n2),
      fCanvas(0),
      fBody(0)
  {
    gStyle->SetPalette(1);
    gStyle->SetTitleX(.50);
    gStyle->SetTitleY(.99);
    gStyle->SetTitleW(.85);
    gStyle->SetTitleH(.085);
    gStyle->SetTitleAlign(23);
    gStyle->SetTitleFillColor(kWhite);
    gStyle->SetTitleBorderSize(1);
  }
  //____________________________________________________________________
  /** 
   * Open it
   * 
   */
  void Open() 
  {
    fCanvas = new TCanvas(fName, fTitle, 800, TMath::Sqrt(2)*800);
    fCanvas->SetFillColor(0);
    fCanvas->SetLeftMargin(0.15);
    fCanvas->SetBottomMargin(0.15);
    
    fCanvas->Print("comparison.pdf[", "pdf");
  }
  //____________________________________________________________________
  /** 
   * Clear pad 
   * 
   * @param nPad  Pad number 
   * @param d     Detector
   * @param r     Ring 
   * @param flush Flush output 
   * 
   * @return Pointer to pad 
   */
  TPad* 
  Clear(UShort_t nPad, UShort_t d, Char_t r, Bool_t flush=false)
  {
    fCanvas->Clear();
    TPad* top = new TPad("top", "Top", 0, .95, 1, 1, 0, 0);
    top->Draw();
    top->cd();

    TLatex* l = new TLatex(.5, .5, Form("%s for FMD%d%c (%s / %s)", 
					fTitle, d, r, fN1, fN2));
    l->SetNDC();
    l->SetTextAlign(22);
    l->SetTextSize(0.3);
    l->Draw();
  
    fCanvas->cd();
    fBody = new TPad("body", "Body", 0, 0, 1, .95, 0, 0);
    fBody->SetTopMargin(0.05);
    fBody->SetRightMargin(0.05);
    Int_t nCol = Int_t(TMath::Sqrt(nPad));
    Int_t nRow = Int_t(Float_t(nPad)/nCol+.5);
    fBody->Divide(nCol, nRow, flush ? 0 : 0.001, flush ? 0 : 0.001);
    fBody->Draw();
    
    return fBody;
  }  
  //____________________________________________________________________
  /** 
   * Change to pad 
   * 
   * @param i Pad number 
   * 
   * @return Pointer to pad 
   */
  TVirtualPad* cd(Int_t i) 
  {
    if (!fBody) return 0;
    
    return fBody->cd(i);
  }
  //____________________________________________________________________
  /** 
   * Print information
   * 
   * @param d Detector
   * @param r Ring 
   * @param extra  more stuff
   */
  void Print(UShort_t d, Char_t r, const char* extra="")
  {
    fCanvas->Print("comparison.pdf", 
		   Form("Title:FMD%d%c %s", d, r, extra));
  }
  //____________________________________________________________________
  /** 
   * Close it 
   * 
   */
  void Close()
  {
    fCanvas->Print("comparison.pdf]", "pdf");
  }    
  //____________________________________________________________________
  /// NAme 
  const char* fName;
  /// Title
  const char* fTitle;
  /// N1
  const char* fN1;
  /// N2
  const char* fN2;
  /// Canvas
  TCanvas*    fCanvas;
  /// Body 
  TPad*       fBody;
};

//======================================================================

/** 
 * 
 * 
 * @param what 
 * @param fn1 
 * @param fn2 
 * @param o1 
 * @param o2 
 *
 * @ingroup pwglf_forward_scripts_corr
 */
void
GetObjects(const char* what, 
	   const char* fn1, const char* fn2, 
	   TObject*&   o1,  TObject*&   o2)
{
  // --- Open files --------------------------------------------------
  TFile* file1 = TFile::Open(fn1, "READ");
  TFile* file2 = TFile::Open(fn2, "READ");

  if (!file1) { 
    Error("CompareSecMaps", "File %s cannot be opened", fn1);
    return;
  }

  if (!file2) { 
    Error("CompareSecMaps", "File %s cannot be opened", fn2);
    return;
  }
  
  // --- Find Objects ------------------------------------------------
  const char* objName = what;
  o1 = file1->Get(objName);
  o2 = file2->Get(objName);
  
  if (!o1) {
    TList* l = static_cast<TList*>(file1->Get("ForwardCorrResults"));
    if (l) o1 = l->FindObject(objName);
  }
  if (!o1) {				   
    Error("CompareSecMaps", "File %s does not contain an object named %s", 
	  fn1, objName);
    return;
  }
  if (!o2) {
    TList* l = static_cast<TList*>(file2->Get("ForwardCorrResults"));
    if (l) o2 = l->FindObject(objName);
  }
  if (!o2) {
    Error("CompareSecMaps", "File %s does not contain an object named %s", 
	  fn2, objName);
    return;
  }
};

/** 
 * 
 * 
 * @param what 
 * @param fn1 
 * @param fn2 
 * @param o1 
 * @param o2 
 *
 * @ingroup pwglf_forward_scripts_corr
 */
void
GetObjects(UShort_t    what, 
	   const char* fn1, const char* fn2, 
	   TObject*&   o1,  TObject*&   o2)
{
  // --- Find Objects ------------------------------------------------
  AliForwardCorrectionManager::ECorrection ewhat = what;
  // (AliForwardCorrectionManager::ECorrection)what;
  const char* objName = 
    AliForwardCorrectionManager::Instance().GetObjectName(ewhat);

  GetObjects(objName,fn1,fn2,o1,o2);
};



//
// EOF
//
