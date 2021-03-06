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
//
// $Id: PrimaryGeneratorMessenger.cc,v 1.1 2010-10-18 15:56:17 maire Exp $
// GEANT4 tag $Name: geant4-09-04-patch-02 $
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorMessenger.hh"

#include "PrimaryGeneratorAction.hh"
#include "ApparatusSpiceTarget.hh"//for BeamPos
#include "DetectorConstruction.hh"//for SPICE target pedestal tunnelling

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* Gun)
    :fAction(Gun)
{
  
    fNumberOfDecayingLaBrDetectorsCmd = new G4UIcmdWithAnInteger("/DetSys/gun/numberOfDecayingLaBrDetectors",this);
    fNumberOfDecayingLaBrDetectorsCmd->SetGuidance("Set the number of radioactive LaBr detectors");
    fNumberOfDecayingLaBrDetectorsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    fEfficiencyEnergyCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/gun/efficiencyEnergy",this);
    fEfficiencyEnergyCmd->SetGuidance("Set gamma efficiency energy");
    fEfficiencyEnergyCmd->SetUnitCategory("Energy");
    fEfficiencyEnergyCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    fEfficiencyDirectionCmd = new G4UIcmdWith3Vector("/DetSys/gun/direction",this);
    fEfficiencyDirectionCmd->SetGuidance("Set momentum direction.");
    fEfficiencyDirectionCmd->SetGuidance("Direction needs not to be a unit vector.");
    fEfficiencyDirectionCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    fEfficiencyPositionCmd = new G4UIcmdWith3VectorAndUnit("/DetSys/gun/position",this);
    fEfficiencyPositionCmd->SetGuidance("Set position.");
    fEfficiencyPositionCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    fEfficiencyParticleCmd = new G4UIcmdWithAString("/DetSys/gun/particle",this);
    fEfficiencyParticleCmd->SetGuidance("Set particle.");
    fEfficiencyParticleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    fEfficiencyPolarizationCmd = new G4UIcmdWith3Vector("/DetSys/gun/polarization",this);
    fEfficiencyPolarizationCmd->SetGuidance("Set gamma polarization direction.");
    fEfficiencyPolarizationCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    fEfficiencyBeamRadiusCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/gun/beamRadius",this);
    fEfficiencyBeamRadiusCmd->SetGuidance("Set beam radius");
    fEfficiencyBeamRadiusCmd->SetUnitCategory("Length");
    fEfficiencyBeamRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    fConeRadiusCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/gun/coneRadius",this);//SPICE cone
    fConeRadiusCmd->SetGuidance("Set cone radius");
    fConeRadiusCmd->SetUnitCategory("Length");
    fConeRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    fConeZValueCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/gun/coneZValue",this);//SPICE cone values
    fConeZValueCmd->SetGuidance("Set cone z value");
    fConeZValueCmd->SetUnitCategory("Length");
    fConeZValueCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    fConeRValueCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/gun/coneRValue",this);//SPICE cone values
    fConeRValueCmd->SetGuidance("Set cone r value");
    fConeRValueCmd->SetUnitCategory("Length");
    fConeRValueCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    fConeAngleCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/gun/coneMaxAngle",this);//SPICE cone angle value
    fConeAngleCmd->SetGuidance("Set cone value for outer theta - use deg (0-90)");
    fConeAngleCmd->SetUnitCategory("Angle");
    fConeAngleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    fConeMinAngleCmd = new G4UIcmdWithADoubleAndUnit("/DetSys/gun/coneMinAngle",this);//SPICE cone angle value
    fConeMinAngleCmd->SetGuidance("Set cone value for inner theta - use deg (0-90) - default is 0 if none specified");
    fConeMinAngleCmd->SetUnitCategory("Angle");
    fConeMinAngleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    fBeamDistroCmd = new G4UIcmdWithABool("/Detsys/gun/BeamDistro",this);//with target, can apply a distribution
    fBeamDistroCmd->SetGuidance("Set beam ditribution wihtin a target");
    fBeamDistroCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    fBeamFileCmd = new G4UIcmdWithABool("/Detsys/gun/FileDistro",this);//with target, can apply a distribution
    fBeamFileCmd->SetGuidance("Set beam ditribution within a target using definitions in a data file");
    fBeamFileCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    fSourceBeamCmd = new G4UIcmdWithAString("/Detsys/gun/SourceBeam",this);//apply beam make-up from bismuth/barium
    fSourceBeamCmd->SetGuidance("Set beam distribution from source, named by the command");
    fSourceBeamCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger() {
    delete fNumberOfDecayingLaBrDetectorsCmd;
    delete fEfficiencyEnergyCmd;
    delete fEfficiencyDirectionCmd;
    delete fEfficiencyPolarizationCmd;
    delete fEfficiencyBeamRadiusCmd;
    delete fConeRadiusCmd;
    delete fConeZValueCmd;
    delete fConeRValueCmd;
    delete fConeAngleCmd;
    delete fConeMinAngleCmd;
    delete fBeamDistroCmd;
    delete fBeamFileCmd;
    delete fSourceBeamCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == fNumberOfDecayingLaBrDetectorsCmd) {
        fAction->SetNumberOfDecayingLaBrDetectors(fNumberOfDecayingLaBrDetectorsCmd->GetNewIntValue(newValue));
		  return;
    }
    if(command == fEfficiencyEnergyCmd ) {
        fAction->SetEfficiencyEnergy(fEfficiencyEnergyCmd->GetNewDoubleValue(newValue));
	G4cout << fEfficiencyEnergyCmd->GetNewDoubleValue(newValue) << " <- Input beam energy to PGM" << G4endl;
	fAction->SendBeamEnergyToHist(fEfficiencyEnergyCmd->GetNewDoubleValue(newValue));
		  return;
    }
    if( command == fEfficiencyDirectionCmd ) {
        fAction->SetEfficiencyDirection(fEfficiencyDirectionCmd->GetNew3VectorValue(newValue));
		  return;
    }
    if( command == fEfficiencyPositionCmd ) {
        fAction->SetEfficiencyPosition(fEfficiencyPositionCmd->GetNew3VectorValue(newValue));
	fAction->PassTarget(fEfficiencyPositionCmd->GetNew3VectorValue(newValue).z()); 
		  return;
    }
    if( command == fEfficiencyParticleCmd ) {
        fAction->SetEfficiencyParticle(newValue);
		  return;
    }
    if( command == fEfficiencyPolarizationCmd ) {
        fAction->SetEfficiencyPolarization(fEfficiencyPolarizationCmd->GetNew3VectorValue(newValue));
		  return;
    }
    if( command == fEfficiencyBeamRadiusCmd ) {
        fAction->SetEfficiencyBeamRadius(fEfficiencyBeamRadiusCmd->GetNewDoubleValue(newValue));
		  return;
    }
    if( command == fConeRadiusCmd ) {
        fAction->SetConeRadius(fConeRadiusCmd->GetNewDoubleValue(newValue));
	G4cout << "Cone Beam selected" << G4endl;
		  return;
    }
    if( command == fConeZValueCmd ) {
        fAction->SetConeZValue(fConeZValueCmd->GetNewDoubleValue(newValue));
	G4cout << "Cone Beam Z value selected" << G4endl;
		  return;
    }
    if( command == fConeRValueCmd ) {
        fAction->SetConeRValue(fConeRValueCmd->GetNewDoubleValue(newValue));
	G4cout << "Cone Beam R value selected" << G4endl;
		  return;
    }
    if( command == fConeAngleCmd ) {
        fAction->SetConeMaxAngle(fConeAngleCmd->GetNewDoubleValue(newValue));
	G4cout << "Cone Beam via Angle selected" << G4endl;
		  return;
    }
    if( command == fConeMinAngleCmd ) {
        fAction->SetConeMinAngle(fConeMinAngleCmd->GetNewDoubleValue(newValue));
	G4cout << "Cone Beam minimum angle supplied" << G4endl;
		  return;
    }
    if( command == fBeamDistroCmd ) {
	fAction->SetNeedBeamDistro(fBeamDistroCmd->GetNewBoolValue(newValue));
	G4cout << "Beam Distribution within SPICE target selected"  << fBeamDistroCmd->GetNewBoolValue(newValue) << G4endl;
    }
    if( command == fBeamFileCmd ) {
	fAction->SetNeedFileDistro(fBeamFileCmd->GetNewBoolValue(newValue));
	G4cout << "Beam Distribution within SPICE target selected "<< G4endl;
	fAction->PrepareBeamFile();
    }
    if( command == fSourceBeamCmd ) {
	fAction->SetSourceNeeded(true);
	G4cout << "Source beam chosen"<< G4endl;
	fAction->SetSourceName(newValue);
    }  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
