
AliAnalysisTaskEffContBF *AddTaskBalanceEffCont( TString  centralityEstimator="V0M",
						  Double_t centrMin=10.,
						  Double_t centrMax=80.,
						  Double_t vertexZ=10.,
						  Int_t AODfilterBit = 96,
						  TString fileNameBase="AnalysisResults",
						  UInt_t triggerSel = AliVEvent::kINT7,
						  Bool_t usePID=kTRUE,
						  Bool_t usePIDnSigmaComb=kTRUE,
						  Double_t BayesThr = 0.8,
						  AliPID::EParticleType particleType = AliPID::kPion) {
    
    // Creates a balance function analysis task and adds it to the analysis manager.
    // Get the pointer to the existing analysis manager via the static access method.
    TString centralityName("");
    centralityName+=Form("%.0f-%.0f_%.0f",centrMin,centrMax,vertexZ);
    TString outputFileName(fileNameBase);
    outputFileName.Append(".root");
    
    //===========================================================================
    AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) {
        ::Error("AddTaskTriggeredBF", "No analysis manager to connect to.");
        return NULL;
    }
    
    // Check the analysis type using the event handlers connected to the analysis manager.
    //===========================================================================
    if (!mgr->GetInputEventHandler()) {
        ::Error("AddTaskTriggeredBF", "This task requires an input event handler");
        return NULL;
    }
    TString analysisType = mgr->GetInputEventHandler()->GetDataType(); // can be "ESD" or "AOD"
    if(dynamic_cast<AliMCEventHandler*> (AliAnalysisManager::GetAnalysisManager()->GetMCtruthEventHandler())) analysisType = "MC";
    
    
    // Create the task, add it to manager and configure it.
    //===========================================================================
    
    AliAnalysisTaskEffContBF *taskEffContBF = new AliAnalysisTaskEffContBF("TaskEffContBF");
 
    // centrality
    if(centralityEstimator) {
        taskEffContBF->UseCentrality();
        taskEffContBF->SetCentralityEstimator(centralityEstimator);
        taskEffContBF->SetCentralityPercentileRange(centrMin,centrMax);
    }
    
    taskEffContBF->SelectCollisionCandidates(triggerSel);
    
    // vertex
    taskEffContBF->SetVertexDiamond(1.,1.,vertexZ);
    taskEffContBF->SetRejectInjectedSignalsGenName("Hijing");
 
    //analysis kinematic cuts
    taskEffContBF->SetMinPt(0.0);
    taskEffContBF->SetMaxPt(20.0); //5.0
    //taskEffContBF->SetEtaRange(-0.8,0.8,100,0.0,1.6, 64); //acceptance cuts
    //taskEffContBF->SetPtRange(0.1, 20.0, 100);  //acceptance cuts //5.0,49
    taskEffContBF->SetEtaRange(-0.8,0.8,100,0.0,1.6, 64); //acceptance cuts
    taskEffContBF->SetPtRange(0.0, 20.0, 100);  //acceptance cuts //5.0,49
    

    
    TString pidsuffix ="ch";
    if (usePID) {

        pidsuffix = AliPID::ParticleShortName(particleType);
        taskEffContBF->SetUseParticleID(usePID, particleType);
        taskEffContBF->SetUsePIDnSigmaComb(usePIDnSigmaComb);
        taskEffContBF->SetBayesPIDThr(BayesThr);
    }
    
    //AODs
    taskEffContBF->SetAODtrackCutBit(AODfilterBit);
    mgr->AddTask(taskEffContBF);
   
    // Create ONLY the output containers for the data produced by the task.
    // Get and connect other common input/output containers via the manager as below
    //==============================================================================
    //TString outputFileName = AliAnalysisManager::GetCommonFileName();
    outputFileName += ":PWGCFEbyE.outputBalanceFunctionEffContAnalysis";
    AliAnalysisDataContainer *coutQA = mgr->CreateContainer(Form("listQA_%s_%s",centralityName.Data(), pidsuffix.Data()), TList::Class(),AliAnalysisManager::kOutputContainer,outputFileName.Data());
    AliAnalysisDataContainer *coutEffContBF = mgr->CreateContainer(Form("listEffContBF_%s_%s",centralityName.Data(), pidsuffix.Data()), TList::Class(),AliAnalysisManager::kOutputContainer,outputFileName.Data());
    
    mgr->ConnectInput(taskEffContBF, 0, mgr->GetCommonInputContainer());
    mgr->ConnectOutput(taskEffContBF, 1, coutQA);
    mgr->ConnectOutput(taskEffContBF, 2, coutEffContBF);
    
    return taskEffContBF;
}
