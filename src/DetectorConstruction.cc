
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
/// \file analysis/shared/src/DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
//
// $Id: DetectorConstruction.cc 77256 2013-11-22 10:10:23Z gcosmo $
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"


//#include "DetectionSystemGammaTracking.hh"
#include "DetectionSystem8pi.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "DetectionSystemDescant.hh"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ApparatusDescantStructure.hh"

#include "DetectionSystemTestcan.hh"

#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
#include "G4TransportationManager.hh"
#include "NonUniformMagneticField.hh"

#include "DetectionSystemGriffin.hh"
#include "DetectionSystemSceptar.hh"
#include "DetectionSystemSpice.hh"
#include "DetectionSystemPaces.hh"
#include "DetectionSystemSodiumIodide.hh"
#include "DetectionSystemLanthanumBromide.hh"
#include "ApparatusGenericTarget.hh"
#include "ApparatusSpiceTarget.hh"
#include "ApparatusSpiceTargetChamber.hh"
#include "Apparatus8piVacuumChamber.hh"
#include "Apparatus8piVacuumChamberAuxMatShell.hh"
#include "ApparatusGriffinStructure.hh"

#include "DetectionSystemBox.hh" // New file
#include "DetectionSystemGrid.hh"

#include "DetectionSystemAncillaryBGO.hh"

#include "HistoManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction() :
  fSolidWorld(NULL),
  fLogicWorld(NULL),
  fPhysiWorld(NULL)
{
  fWorldSizeX  = fWorldSizeY = fWorldSizeZ = 10.0*m;

  fBoxMat = "G4_WATER";
  fBoxThickness = 0.0*mm;
  fBoxInnerDimensions = G4ThreeVector(0.0*mm,0.0*mm,0.0*mm);
  fBoxColour = G4ThreeVector(0.0,0.0,1.0);

  fGridMat = "G4_WATER";
  fGridSize = 0.0*mm;
  fGridDimensions = G4ThreeVector(0.0*mm,0.0*mm,0.0*mm);
  fGridColour = G4ThreeVector(1.0,0.0,0.0);

  // materials
  DefineMaterials();

  //  builtDetectors = false;


  fMatWorldName = "G4_AIR";

  // Generic Target Apparatus
  fSetGenericTargetMaterial   = false;
  fSetGenericTargetDimensions = false;
  fSetGenericTargetPosition   = false;

  // Field Box
  fSetFieldBoxMaterial= false;//think this has been removed 17/8
  fSetFieldBoxDimensions= false;
  fSetFieldBoxPosition= false;
  fSetFieldBoxMagneticField= false;

  // parameters to suppress:

  DefineSuppressedParameters();

  // Shield Selection Default

  fUseTigressPositions = false;

  fDetectorShieldSelect = 1 ; // Include suppressors by default.
  fExtensionSuppressorLocation = 0 ; // Back by default (Detector Forward)
  fHevimetSelector = 0 ; // Chooses whether or not to include a hevimet

  fCustomDetectorNumber 		= 1 ; // detNum
  fCustomDetectorPosition  = 1 ; // posNum
  fCustomDetectorVal				= 0 ; // Unused for now (Oct 2013)


  // create commands for interactive definition

  fDetectorMessenger = new DetectorMessenger(this);
 
  // ensure the global field is initialized
  //(void)GlobalField::getObject();

  //expHallMagField = new MagneticField(); // Global field is set to zero

  fGriffinDetectorsMapIndex = 0;
  for(G4int i = 0; i < 16; i++)
    {
		fGriffinDetectorsMap[i] = 0;
    }

  fDescantColor = "white";
  fDescantRotation.setX(M_PI);
  fDescantRotation.setY(0.);
  fDescantRotation.setZ(0.);
  
  fSpiceTargetThickness=0.;//otherwise when called will be a random
  fSpiceTargetBackerThickness=0.;
  fSpiceTargetProtectorThickness=0.;
  
  fSetSpiceIn = false;	  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction() {
  delete fDetectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct() {

  // Replaced by ConstructDetectionSystems

  // Experimental hall (world volume)
  // search the world material by its name

  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  G4Material* matWorld = G4Material::GetMaterial(fMatWorldName);

  if( !matWorld ) {
	 G4cout << " ----> Material " << fMatWorldName << " not found, cannot build world! " << G4endl;
	 return 0;
  }

  fSolidWorld = new G4Box("World", fWorldSizeX/2,fWorldSizeY/2,fWorldSizeZ/2);

  fLogicWorld = new G4LogicalVolume(fSolidWorld,		//its solid
												matWorld,	//its material
												"World");		//its name

  fPhysiWorld = new G4PVPlacement(   0,                  //no rotation
												 G4ThreeVector(),	//at (0,0,0)
												 fLogicWorld,         //its logical volume
												 "World",            //its name
												 0,                  //its mother  volume
												 false,              //no boolean operation
												 0);                 //copy number

  // Visualization Attributes

  fLogicWorld->SetVisAttributes (G4VisAttributes::Invisible); // The following block of code works too.

  //  G4VisAttributes* worldVisAtt = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
  //  worldVisAtt->SetForceWireframe(true);
  //  worldVisAtt->SetVisibility(worldVis);
  //  fLogicWorld->SetVisAttributes(worldVisAtt);
  //  fLogicWorld = fLogicWorld;

  return fPhysiWorld;
}

void DetectorConstruction::SetWorldMaterial( G4String name ) {
  fMatWorldName = name;
  UpdateGeometry(); // auto update
}

void DetectorConstruction::SetWorldDimensions( G4ThreeVector vec ) {
  fWorldSizeX = vec.x() ;
  fWorldSizeY = vec.y() ;
  fWorldSizeZ = vec.z() ;
  UpdateGeometry(); // auto update
}

void DetectorConstruction::SetWorldVis( G4bool vis ) {
  fWorldVis = vis;
  UpdateGeometry(); // auto update
}

//void DetectorConstruction::SetWorldMagneticField( G4ThreeVector vec )
//{
//    //expHallMagField->SetFieldValue(G4ThreeVector(vec.x(),vec.y(),vec.z()));
//}

void DetectorConstruction::SetTabMagneticField(G4String PathAndTableName, G4double z_offset, G4double z_rotation)
{

  //const char * c = PathAndTableName.c_str();///conversion attempt using .c_str() (a string member function)
 new NonUniformMagneticField(PathAndTableName,z_offset,z_rotation);///addition of field for SPICE
}

void DetectorConstruction::UpdateGeometry() {
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}

void DetectorConstruction::SetGenericTargetMaterial( G4String name )
{
  DetectorConstruction::fSetGenericTargetMaterial = true;
  DetectorConstruction::fGenericTargetMaterial = name;
  DetectorConstruction::SetGenericTarget();
}

void DetectorConstruction::SetGenericTargetDimensions( G4ThreeVector vec )
{
  DetectorConstruction::fSetGenericTargetDimensions = true;
  DetectorConstruction::fGenericTargetDimensions = vec;
  DetectorConstruction::SetGenericTarget();
}

void DetectorConstruction::SetGenericTargetPosition( G4ThreeVector vec )
{
  DetectorConstruction::fSetGenericTargetPosition = true;
  DetectorConstruction::fGenericTargetPosition = vec;
  DetectorConstruction::SetGenericTarget();
}

void DetectorConstruction::SetGenericTarget()
{
  if( fSetGenericTargetMaterial ) {
    if( fSetGenericTargetDimensions ) {
	if( fSetGenericTargetPosition ) {
	  G4String name = fGenericTargetMaterial;
	  G4double vecX = fGenericTargetDimensions.x()/mm;
	  G4double vecY = fGenericTargetDimensions.y()/mm;
	  G4double vecZ = fGenericTargetDimensions.z()/mm;
	  
	  ApparatusGenericTarget* pApparatusGenericTarget = new ApparatusGenericTarget();
	  pApparatusGenericTarget->Build(name, vecX, vecY, vecZ);
	  G4RotationMatrix* rotate = new G4RotationMatrix;
	  pApparatusGenericTarget->PlaceApparatus(fLogicWorld, fGenericTargetPosition, rotate);
	}
    }
  }
}
//*******************
//** SPICE targets **
//*******************
void DetectorConstruction::SetSpiceTargetMaterial( G4String name)
{
  fSetSpiceTargetMaterial = true;
  fSpiceTargetMaterial = name; 
  SetSpiceTarget();
}

void DetectorConstruction::SetSpiceTargetThickness( G4double surface_density )
{
  fSetSpiceTargetThickness = true;
  fSpiceTargetThickness = surface_density;
}

void DetectorConstruction::SetSpiceTargetDensity( G4double density)
{
  fSetSpiceTargetDensity = true;
  fSpiceTargetDensity = density;
}

void DetectorConstruction::SetSpiceTarget()
{
  if( fSetSpiceTargetMaterial ) {
    if( fSetSpiceTargetThickness ) {
	if( fSetSpiceTargetDensity ) {
	  G4String name = fSpiceTargetMaterial;
	  G4double surface_density = fSpiceTargetThickness/(mg/cm2);
	  G4double density = fSpiceTargetDensity/(g/cm3);
	  
	  fApparatusSpiceTarget->BuildTarget(name, surface_density, density);
	  fApparatusSpiceTarget->PlaceTarget(fLogicWorld);
	}
    }
  }
}

void DetectorConstruction::SetSpiceTargetBackerMaterial( G4String name )
{
  fSetSpiceTargetBackerMaterial = true;
  fSpiceTargetBackerMaterial = name;
  SetSpiceBackerTarget();//should be last one given, may add to all to allow for any ordered commands
}

void DetectorConstruction::SetSpiceTargetBackerThickness( G4double surface_density )
{
  fSetSpiceTargetBackerThickness = true;
  fSpiceTargetBackerThickness = surface_density;
}

void DetectorConstruction::SetSpiceTargetBackerDensity( G4double density )
{
  fSetSpiceTargetBackerDensity = true;
  fSpiceTargetBackerDensity = density;
}

void DetectorConstruction::SetSpiceBackerTarget( )
{
  if( fSetSpiceTargetBackerMaterial ) {
    if( fSetSpiceTargetBackerThickness ) {
	if( fSetSpiceTargetBackerDensity ) {
	  G4String name = fSpiceTargetBackerMaterial;
	  G4double surface_density = fSpiceTargetBackerThickness/(mg/cm2);
	  G4double density = fSpiceTargetBackerDensity/(g/cm3);
	  
	  fApparatusSpiceTarget->BuildBacker(name, surface_density, density);
	  fApparatusSpiceTarget->PlaceTargetBacker(fLogicWorld);
	}
    }
  }
}
void DetectorConstruction::SetSpiceTargetProtectorMaterial( G4String name )
{
  fSetSpiceTargetProtectorMaterial = true;
  fSpiceTargetProtectorMaterial = name;
  SetSpiceProtectorTarget();
}

void DetectorConstruction::SetSpiceTargetProtectorThickness( G4double surface_density )
{
  fSetSpiceTargetProtectorThickness = true;
  fSpiceTargetProtectorThickness = surface_density;
}

void DetectorConstruction::SetSpiceTargetProtectorDensity( G4double density )
{
  fSetSpiceTargetProtectorDensity = true;
  fSpiceTargetProtectorDensity = density;
}

void DetectorConstruction::SetSpiceProtectorTarget()
{
  if( fSetSpiceTargetProtectorMaterial ) {
    if( fSetSpiceTargetProtectorThickness ) {
	if( fSetSpiceTargetProtectorDensity ) {
	  G4String name = fSpiceTargetProtectorMaterial;
	  G4double surface_density = fSpiceTargetProtectorThickness/(mg/cm2);
	  G4double density = fSpiceTargetProtectorDensity/(g/cm3);
	  
	  fApparatusSpiceTarget->BuildProtector(name, surface_density, density);
	  fApparatusSpiceTarget->PlaceTargetProtector(fLogicWorld);
	}
    }
  }
}

void DetectorConstruction::SetSpiceSource(G4bool need){
  if(need == true){
    fApparatusSpiceTarget->BuildBracket();
    fApparatusSpiceTarget->PlaceBracket(fLogicWorld);
    fApparatusSpiceTarget->BuildHolder();
    fApparatusSpiceTarget->PlaceHolder(fLogicWorld);
  }
}

//void DetectorConstruction::SetFieldBoxMaterial( G4String name )
//{
//  fSetFieldBoxMaterial = true;
//  fieldBoxMaterial = name;
//  SetFieldBox();
//}

//void DetectorConstruction::SetFieldBoxDimensions( G4ThreeVector vec )
//{
//  fSetFieldBoxDimensions = true;
//  fieldBoxDimensions = vec;
//  SetFieldBox();
//}

//void DetectorConstruction::SetFieldBoxPosition( G4ThreeVector vec )
//{
//  fSetFieldBoxPosition = true;
//  fieldBoxPosition = vec;
//  SetFieldBox();
//}

//void DetectorConstruction::SetFieldBoxMagneticField( G4ThreeVector vec )
//{
//  fSetFieldBoxMagneticField = true;
//  fieldBoxMagneticField = vec;
//  SetFieldBox();
//}

//void DetectorConstruction::SetFieldBox( )
//{
//  if( fSetFieldBoxMagneticField && fSetFieldBoxMaterial &&
//        fSetFieldBoxDimensions 	 && fSetFieldBoxPosition   )
//        {
//            G4String name = fieldBoxMaterial;
//            G4double vecX = fieldBoxDimensions.x()/mm;
//            G4double vecY = fieldBoxDimensions.y()/mm;
//            G4double vecZ = fieldBoxDimensions.z()/mm;
//            ApparatusFieldBox* pApparatusFieldBox = new ApparatusFieldBox();
//            pApparatusFieldBox->Build(name, vecX, vecY, vecZ, fieldBoxMagneticField);
//            G4RotationMatrix* rotate = new G4RotationMatrix;
//            pApparatusFieldBox->PlaceApparatus(fLogicWorld, fieldBoxPosition, rotate);
//        }
//}

//void DetectorConstruction::AddBox()
//{
//    if(fBoxThickness != 0.0*mm)
//    {
//        DetectionSystemBox* pBox = new DetectionSystemBox(	fBoxInnerDimensions.x(),
//                                                                                                            fBoxInnerDimensions.y(),
//                                                                                                            fBoxInnerDimensions.z(),
//                                                                                                            fBoxThickness,
//                                                                                                            fBoxMat,
//                                                                                                            fBoxColour ) ;
//        pBox->Build() ;
//        pBox->PlaceDetector( fLogicWorld ) ;
//    }
//}

void DetectorConstruction::AddGrid() {
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  if(fGridSize != 0.0*mm)
    {
		DetectionSystemGrid* pGrid = new DetectionSystemGrid(	fGridDimensions.x(),
																				fGridDimensions.y(),
																				fGridDimensions.z(),
																				fGridSize,
																				fGridMat,
																				fGridColour,
																				fGridOffset) ;
		pGrid->Build();
		pGrid->PlaceDetector( fLogicWorld );
    }
}

void DetectorConstruction::AddApparatusSpiceTargetChamber(G4String MedLo, G4double TargetPedestal)//parameter sets lens for SPICE - should be matched with field
{
   //Create Target Chamber
   ApparatusSpiceTargetChamber* fApparatusSpiceTargetChamber = new ApparatusSpiceTargetChamber(MedLo, TargetPedestal);
   fApparatusSpiceTargetChamber->Build( fLogicWorld );
   fApparatusSpiceTarget = new ApparatusSpiceTarget(targetz);//prepares target instance after the intro of the chamber
   fSetSpiceIn = true;
}

void DetectorConstruction::AddApparatus8piVacuumChamber() {
  //Create Vacuum Chamber
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  Apparatus8piVacuumChamber* pApparatus8piVacuumChamber = new Apparatus8piVacuumChamber();
  pApparatus8piVacuumChamber->Build( fLogicWorld );
}

void DetectorConstruction::AddApparatus8piVacuumChamberAuxMatShell(G4double thickness) {
  //Create Shell Around Vacuum Chamber
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  Apparatus8piVacuumChamberAuxMatShell* pApparatus8piVacuumChamberAuxMatShell = new Apparatus8piVacuumChamberAuxMatShell();
  pApparatus8piVacuumChamberAuxMatShell->Build( fLogicWorld, thickness );
}

void DetectorConstruction::AddApparatusGriffinStructure(G4int selector) {
  //Create Shell Around Vacuum Chamber
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  ApparatusGriffinStructure* pApparatusGriffinStructure = new ApparatusGriffinStructure();
  pApparatusGriffinStructure->Build();

  pApparatusGriffinStructure->Place(fLogicWorld, selector);
}

//void DetectorConstruction::AddDetectionSystemGammaTracking(G4int ndet)
//{
//  // Describe Placement
//  G4ThreeVector direction = G4ThreeVector(0,0,1);
//  G4ThreeVector move = 0.0 * direction;
//  G4RotationMatrix* rotate = new G4RotationMatrix;
//  rotate->rotateX(0.0);
//  rotate->rotateY(0.0);
//  rotate->rotateZ(0.0);

//  G4int detectorNumber = 0;

//    DetectionSystemGammaTracking* pGammaTracking = new DetectionSystemGammaTracking() ;
//    pGammaTracking->Build() ;

//  pGammaTracking->PlaceDetector( fLogicWorld, move, rotate, detectorNumber );
//}

void DetectorConstruction::AddDetectionSystemSodiumIodide(G4int ndet) {
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  // Describe Placement
  G4double detectorAngles[8][2] = {{}};
  G4double theta,phi,position;
  G4ThreeVector move,direction;

  detectorAngles[0][0] 	= 0.0;
  detectorAngles[1][0] 	= 45.0;
  detectorAngles[2][0] 	= 90.0;
  detectorAngles[3][0] 	= 135.0;
  detectorAngles[4][0] 	= 180.0;
  detectorAngles[5][0] 	= 225.0;
  detectorAngles[6][0] 	= 270.0;
  detectorAngles[7][0] 	= 315.0;
  detectorAngles[0][1] 	= 90.0;
  detectorAngles[1][1] 	= 90.0;
  detectorAngles[2][1] 	= 90.0;
  detectorAngles[3][1] 	= 90.0;
  detectorAngles[4][1] 	= 90.0;
  detectorAngles[5][1] 	= 90.0;
  detectorAngles[6][1] 	= 90.0;
  detectorAngles[7][1] 	= 90.0;

  DetectionSystemSodiumIodide* pSodiumIodide = new DetectionSystemSodiumIodide() ;
  pSodiumIodide->Build() ;

  for(G4int detectorNumber = 0; detectorNumber < ndet; detectorNumber++)
    {
		phi = detectorAngles[detectorNumber][0]*deg; // Creates a ring in phi plane
		theta = detectorAngles[detectorNumber][1]*deg;

		direction = G4ThreeVector(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
		position = 25.0*cm + (pSodiumIodide->GetDetectorLengthOfUnitsCM()/2.0);
		move = position * direction;

		G4RotationMatrix* rotate = new G4RotationMatrix; 		//rotation matrix corresponding to direction vector
		rotate->rotateX(theta);
		rotate->rotateY(0);
		rotate->rotateZ(phi+0.5*M_PI);

		pSodiumIodide->PlaceDetector( fLogicWorld, move, rotate, detectorNumber ) ;
    }

  HistoManager::Instance().NaI(true);
}

void DetectorConstruction::AddDetectionSystemLanthanumBromide(G4ThreeVector input) {
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  G4int ndet = G4int(input.x());
  G4double radialpos = input.y()*cm;

  DetectionSystemLanthanumBromide* pDetectionSystemLanthanumBromide = new DetectionSystemLanthanumBromide();
  pDetectionSystemLanthanumBromide->Build();

  for(G4int detectorNumber = 0; detectorNumber < ndet; detectorNumber++)
    {
		pDetectionSystemLanthanumBromide->PlaceDetector(fLogicWorld, detectorNumber, radialpos);
    }
  HistoManager::Instance().LaBr(true);
}

void DetectorConstruction::AddDetectionSystemAncillaryBGO(G4ThreeVector input) {
  if(fLogicWorld == NULL) {
	 Construct();
  }

  G4int ndet = G4int(input.x());
  G4double radialpos = input.y()*cm;
  G4int hevimetopt = G4int(input.z());

  DetectionSystemAncillaryBGO* pDetectionSystemAncillaryBGO = new DetectionSystemAncillaryBGO();
  pDetectionSystemAncillaryBGO->Build();

  for(G4int detectorNumber = 0; detectorNumber < ndet; detectorNumber++)
    {
		pDetectionSystemAncillaryBGO->PlaceDetector(fLogicWorld, detectorNumber, radialpos, hevimetopt);
    }
  HistoManager::Instance().AncBgo(true);
}



G4double DetectorConstruction::GetLanthanumBromideCrystalRadius() {
  DetectionSystemLanthanumBromide* pDetectionSystemLanthanumBromide = new DetectionSystemLanthanumBromide();
  G4double out = pDetectionSystemLanthanumBromide->GetCrystalRadius();
  return out;
}

G4double DetectorConstruction::GetLanthanumBromideCrystalLength() {
  DetectionSystemLanthanumBromide* pDetectionSystemLanthanumBromide = new DetectionSystemLanthanumBromide();
  G4double out = pDetectionSystemLanthanumBromide->GetCrystalLength();
  return out;
}

G4double DetectorConstruction::GetLanthanumBromideR() {
  DetectionSystemLanthanumBromide* pDetectionSystemLanthanumBromide = new DetectionSystemLanthanumBromide();
  G4double out = pDetectionSystemLanthanumBromide->GetR();
  return out;
}

G4double DetectorConstruction::GetLanthanumBromideTheta(G4int i) {
  DetectionSystemLanthanumBromide* pDetectionSystemLanthanumBromide = new DetectionSystemLanthanumBromide();
  G4double out = pDetectionSystemLanthanumBromide->GetTheta(i);
  return out;
}

G4double DetectorConstruction::GetLanthanumBromidePhi(G4int i) {
  DetectionSystemLanthanumBromide* pDetectionSystemLanthanumBromide = new DetectionSystemLanthanumBromide();
  G4double out = pDetectionSystemLanthanumBromide->GetPhi(i);
  return out;
}
G4double DetectorConstruction::GetLanthanumBromideYaw(G4int i) {
  DetectionSystemLanthanumBromide* pDetectionSystemLanthanumBromide = new DetectionSystemLanthanumBromide();
  G4double out = pDetectionSystemLanthanumBromide->GetYaw(i);
  return out;
}

G4double DetectorConstruction::GetLanthanumBromidePitch(G4int i) {
  DetectionSystemLanthanumBromide* pDetectionSystemLanthanumBromide = new DetectionSystemLanthanumBromide();
  G4double out = pDetectionSystemLanthanumBromide->GetPitch(i);
  return out;
}

G4double DetectorConstruction::GetLanthanumBromideRoll(G4int i) {
  DetectionSystemLanthanumBromide* pDetectionSystemLanthanumBromide = new DetectionSystemLanthanumBromide();
  G4double out = pDetectionSystemLanthanumBromide->GetRoll(i);
  return out;
}

G4double DetectorConstruction::GetLanthanumBromideCrystalRadialPosition() {
  DetectionSystemLanthanumBromide* pDetectionSystemLanthanumBromide = new DetectionSystemLanthanumBromide();
  G4double out = pDetectionSystemLanthanumBromide->GetCrystalRadialPosition();
  return out;
}


// Temporary Function for testing purposes
void DetectorConstruction::AddDetectionSystemGriffinCustomDetector(G4int) {
	if(fLogicWorld == NULL) {
		Construct();
	}

	fGriffinDetectorsMap[fGriffinDetectorsMapIndex] = fCustomDetectorNumber ;
	fGriffinDetectorsMapIndex++;

	// NOTE: ndet served no purpose in this case but I left it in just in case this needs to be modified later. The position of a detector placed using this function must be set using
	// SetDeadLayer.

	DetectionSystemGriffin* pGriffinCustom = new DetectionSystemGriffin( fExtensionSuppressorLocation , fDetectorShieldSelect, fDetectorRadialDistance, fHevimetSelector ); // Select Forward (0) or Back (1)

	pGriffinCustom->BuildDeadLayerSpecificCrystal(fCustomDetectorNumber-1);

	pGriffinCustom->PlaceDeadLayerSpecificCrystal( fLogicWorld, fCustomDetectorNumber-1, fCustomDetectorPosition-1, fUseTigressPositions ) ;

	pGriffinCustom->BuildEverythingButCrystals();

	pGriffinCustom->PlaceEverythingButCrystals( fLogicWorld, fCustomDetectorNumber-1, fCustomDetectorPosition-1, fUseTigressPositions ) ;

	HistoManager::Instance().Griffin(true);
}

void DetectorConstruction::AddDetectionSystemGriffinCustom(G4int ndet) {
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  G4int detNum;
  G4int posNum;

  for( detNum = 1; detNum <= ndet; detNum++ ) {
	 posNum = detNum;

	 fGriffinDetectorsMap[fGriffinDetectorsMapIndex] = detNum;
	 fGriffinDetectorsMapIndex++;

	 DetectionSystemGriffin* pGriffinCustom = new DetectionSystemGriffin( fExtensionSuppressorLocation,  fDetectorShieldSelect ,  fDetectorRadialDistance, fHevimetSelector ) ; // Select Forward (0) or Back (1)

	 pGriffinCustom->BuildDeadLayerSpecificCrystal(detNum-1);
	 pGriffinCustom->PlaceDeadLayerSpecificCrystal( fLogicWorld, detNum-1, posNum-1, fUseTigressPositions ) ;
	 pGriffinCustom->BuildEverythingButCrystals();
	 pGriffinCustom->PlaceEverythingButCrystals( fLogicWorld, detNum-1, posNum-1, fUseTigressPositions ) ;

  }
  HistoManager::Instance().Griffin(true);
}

void DetectorConstruction::AddDetectionSystemGriffinShieldSelect( G4int ShieldSelect ){
  fDetectorShieldSelect = ShieldSelect ;
}

void DetectorConstruction::AddDetectionSystemGriffinSetRadialDistance( G4double detectorDist ){
  fDetectorRadialDistance = detectorDist ;
}

void DetectorConstruction::AddDetectionSystemGriffinSetExtensionSuppLocation( G4int detectorPos ){
  fExtensionSuppressorLocation = detectorPos ;
}

void DetectorConstruction::AddDetectionSystemGriffinSetDeadLayer( G4ThreeVector params ) {

  fCustomDetectorNumber 		= (G4int)params.x(); // detNum
  fCustomDetectorPosition  = (G4int)params.y(); // posNum
  fCustomDetectorVal			  = (G4int)params.z(); // Unused at the moment.

}

void DetectorConstruction::AddDetectionSystemGriffinForward(G4int ndet) {
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  //  G4double theta,phi,position;
  //  G4ThreeVector move,direction;

  //  DetectionSystemGriffin* pGriffinForward = new DetectionSystemGriffin(0, 1, fGriffinFwdBackPosition); // Select Forward (0) or Back (1)
  //  pGriffinForward->Build();

  //  for( G4int detectorNumber = 0; detectorNumber < ndet; detectorNumber++ )
  //  {
  //    direction = G4ThreeVector(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
  //    position = fGriffinFwdBackPosition;
  //    move = position * direction;

  //    G4RotationMatrix* rotate = new G4RotationMatrix; 		//rotation matrix corresponding to direction vector

  //    pGriffinForward->PlaceDetector( fLogicWorld, move, rotate, detectorNumber ) ;
  //  }

  G4int detNum;
  G4int posNum;
  G4int config  = 0;
  for( detNum = 1; detNum <= ndet; detNum++ ) {
	 posNum = detNum;

	 fGriffinDetectorsMap[fGriffinDetectorsMapIndex] = detNum;
	 fGriffinDetectorsMapIndex++;

	 DetectionSystemGriffin* pGriffinDLS = new DetectionSystemGriffin(config, 1, fGriffinFwdBackPosition, fHevimetSelector); // Select Forward (0) or Back (1)

	 pGriffinDLS->BuildDeadLayerSpecificCrystal(detNum-1);
	 pGriffinDLS->PlaceDeadLayerSpecificCrystal( fLogicWorld, detNum-1, posNum-1, fUseTigressPositions ) ;
	 pGriffinDLS->BuildEverythingButCrystals();
	 pGriffinDLS->PlaceEverythingButCrystals( fLogicWorld, detNum-1, posNum-1, fUseTigressPositions ) ;
  }
  HistoManager::Instance().Griffin(true);
}

void DetectorConstruction::AddDetectionSystemGriffinForwardDetector(G4int ndet) {
  //  G4double theta,phi,position;
  //  G4ThreeVector move,direction;


  //  DetectionSystemGriffin* pGriffinForward = new DetectionSystemGriffin(0, 1, fGriffinFwdBackPosition); // Select Forward (0) or Back (1)
  //  pGriffinForward->Build();

  //  direction = G4ThreeVector(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
  //  position = fGriffinFwdBackPosition;
  //  move = position * direction;

  //  G4RotationMatrix* rotate = new G4RotationMatrix; 		//rotation matrix corresponding to direction vector

  //  pGriffinForward->PlaceDetector( fLogicWorld, move, rotate, ndet ) ;
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  G4int detNum = ndet;
  G4int posNum = ndet;
  G4int config  = 0;
  fGriffinDetectorsMap[fGriffinDetectorsMapIndex] = detNum;
  fGriffinDetectorsMapIndex++;


  DetectionSystemGriffin* pGriffinDLS = new DetectionSystemGriffin(config, 1, fGriffinFwdBackPosition, fHevimetSelector ); // Select Forward (0) or Back (1)


  pGriffinDLS->BuildDeadLayerSpecificCrystal(detNum-1);

  pGriffinDLS->PlaceDeadLayerSpecificCrystal( fLogicWorld, detNum-1, posNum-1, fUseTigressPositions ) ;

  pGriffinDLS->BuildEverythingButCrystals();

  pGriffinDLS->PlaceEverythingButCrystals( fLogicWorld, detNum-1, posNum-1, fUseTigressPositions ) ;

  HistoManager::Instance().Griffin(true);
}

void DetectorConstruction::AddDetectionSystemGriffinBack(G4int ndet) {
  //  G4double theta,phi,position;
  //  G4ThreeVector move,direction;


  //  DetectionSystemGriffin* pGriffinBack = new DetectionSystemGriffin(1, 1, fGriffinFwdBackPosition ) ; // Select Forward (0) or Back (1)
  //  pGriffinBack->Build();

  //  for(G4int detectorNumber = 0; detectorNumber < ndet; detectorNumber++)
  //  {
  //    direction = G4ThreeVector(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
  //    position = fGriffinFwdBackPosition;
  //    move = position * direction;

  //    G4RotationMatrix* rotate = new G4RotationMatrix; 		//rotation matrix corresponding to direction vector

  //    pGriffinBack->PlaceDetector( fLogicWorld, move, rotate, detectorNumber ) ;
  //  }
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  G4int detNum;
  G4int posNum;
  G4int config  = 1;

  for( detNum = 1; detNum <= ndet; detNum++ ) {
	 posNum = detNum;

	 fGriffinDetectorsMap[fGriffinDetectorsMapIndex] = detNum;
	 fGriffinDetectorsMapIndex++;

	 DetectionSystemGriffin* pGriffinDLS = new DetectionSystemGriffin(config, 1, fGriffinFwdBackPosition, fHevimetSelector ); // Select Forward (0) or Back (1)

	 pGriffinDLS->BuildDeadLayerSpecificCrystal(detNum-1);
	 pGriffinDLS->PlaceDeadLayerSpecificCrystal( fLogicWorld, detNum-1, posNum-1, fUseTigressPositions ) ;
	 pGriffinDLS->BuildEverythingButCrystals();
	 pGriffinDLS->PlaceEverythingButCrystals( fLogicWorld, detNum-1, posNum-1, fUseTigressPositions ) ;

  }

  HistoManager::Instance().Griffin(true);
}

void DetectorConstruction::AddDetectionSystemGriffinBackDetector(G4int ndet) {
  //  G4double theta,phi,position;
  //  G4ThreeVector move,direction;


  //  DetectionSystemGriffin* pGriffinBack = new DetectionSystemGriffin(1, 1, fGriffinFwdBackPosition ); // Select Forward (0) or Back (1)
  //  pGriffinBack->Build();

  //  direction = G4ThreeVector(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
  //  position = fGriffinFwdBackPosition;
  //  move = position * direction;

  //  G4RotationMatrix* rotate = new G4RotationMatrix; 		//rotation matrix corresponding to direction vector

  //  pGriffinBack->PlaceDetector( fLogicWorld, move, rotate, ndet ) ;
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  G4int detNum = ndet;
  G4int posNum = ndet;
  G4int config  = 1;

  fGriffinDetectorsMap[fGriffinDetectorsMapIndex] = detNum;
  fGriffinDetectorsMapIndex++;

  DetectionSystemGriffin* pGriffinDLS = new DetectionSystemGriffin(config, 1, fGriffinFwdBackPosition, fHevimetSelector); // Select Forward (0) or Back (1)

  pGriffinDLS->BuildDeadLayerSpecificCrystal(detNum-1);
  pGriffinDLS->PlaceDeadLayerSpecificCrystal( fLogicWorld, detNum-1, posNum-1, fUseTigressPositions ) ;
  pGriffinDLS->BuildEverythingButCrystals();
  pGriffinDLS->PlaceEverythingButCrystals( fLogicWorld, detNum-1, posNum-1, fUseTigressPositions ) ;

  HistoManager::Instance().Griffin(true);
}

void DetectorConstruction::AddDetectionSystemGriffinHevimet(G4int input) {
  // Includes hevimet.
  fHevimetSelector = input ;

}

// This will be reaplced with the addGriffinCustomDetector function. The dead layer must be set using
// the SetCustomDeadLayer command. This will take longer for many different detectors in different configurations,
// but it is possible to place multiple custom detectors using addGriffinCustom as well.
//void DetectorConstruction::AddDetectionSystemGriffinPositionConfig(G4ThreeVector input)
//{
//  G4int detNum = (G4int)input.x();
//  G4int posNum = (G4int)input.y();
//  G4int config  = (G4int)input.z();


////  DetectionSystemGriffin* pGriffinBack = new DetectionSystemGriffin( config, 1, fGriffinFwdBackPosition ); // Select Forward (0) or Back (1)
////  pGriffinBack->BuildDeadLayerSpecificDetector(detNum-1);
////  pGriffinBack->PlaceDeadLayerSpecificDetector( fLogicWorld, detNum-1, posNum-1 ) ;

//  fGriffinDetectorsMap[fGriffinDetectorsMapIndex] = detNum;
//  fGriffinDetectorsMapIndex++;

//  DetectionSystemGriffin* pGriffinDLS = new DetectionSystemGriffin(config, 1, fGriffinFwdBackPosition); // Select Forward (0) or Back (1)

//  pGriffinDLS->BuildDeadLayerSpecificCrystal(detNum-1);
//  pGriffinDLS->PlaceDeadLayerSpecificCrystal( fLogicWorld, detNum-1, posNum-1 ) ;
//  pGriffinDLS->BuildEverythingButCrystals();
//  pGriffinDLS->PlaceEverythingButCrystals( fLogicWorld, detNum-1, posNum-1 ) ;

//}


void DetectorConstruction::AddDetectionSystemSceptar(G4int ndet) {
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  DetectionSystemSceptar* pSceptar = new DetectionSystemSceptar() ;
  pSceptar->Build() ;
  pSceptar->PlaceDetector( fLogicWorld, ndet ) ;

  HistoManager::Instance().Sceptar(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DetectorConstruction::AddDetectionSystemDescant(G4int ndet) {
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  DetectionSystemDescant* pDetectionSystemDescant = new DetectionSystemDescant(true) ;
  pDetectionSystemDescant->Build() ;
  pDetectionSystemDescant->PlaceDetector( fLogicWorld, ndet ) ;

  HistoManager::Instance().Descant(true);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DetectorConstruction::AddDetectionSystemDescantAuxPorts(G4ThreeVector input) {
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  G4int ndet = G4int(input.x());
  G4double radialpos = input.y()*cm;
  G4int leadShield = G4bool(input.z());

  if( leadShield ) G4cout << "Building DESCANT detectors WITH lead shielding" << G4endl;
  if( !leadShield )G4cout << "Building DESCANT detectors WITHOUT lead shielding" << G4endl;

  DetectionSystemDescant *  pDetectionSystemDescant = new DetectionSystemDescant(leadShield) ;
  pDetectionSystemDescant->Build() ;

  for(G4int detectorNumber = 0; detectorNumber < ndet; detectorNumber++)
    {
		pDetectionSystemDescant->PlaceDetectorAuxPorts(fLogicWorld, detectorNumber, radialpos);
    }

  HistoManager::Instance().Descant(true);
}

void DetectorConstruction::SetDetectionSystemDescantRotation(G4ThreeVector input) {
  //convert from degree to rad
  fDescantRotation.setX(input.x()/180.*M_PI);
  fDescantRotation.setY(input.y()/180.*M_PI);
  fDescantRotation.setZ(input.z()/180.*M_PI);
}

void DetectorConstruction::SetDetectionSystemDescantColor(G4String input) {
  fDescantColor = input;
}

void DetectorConstruction::AddDetectionSystemDescantCart(G4ThreeVector input) {
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  DetectionSystemDescant* pDetectionSystemDescant = new DetectionSystemDescant(true) ;
  pDetectionSystemDescant->Build() ;
  pDetectionSystemDescant->PlaceDetector( fLogicWorld, fDescantColor, input, fDescantRotation ) ;

  HistoManager::Instance().Descant(true);
}

void DetectorConstruction::AddDetectionSystemDescantSpher(G4ThreeVector input, G4double unit) {
  G4ThreeVector sphericalVector;
  //convert from degree to rad
  sphericalVector.setRThetaPhi(input.x()*unit, input.y()/180.*M_PI, input.z()/180.*M_PI);
  AddDetectionSystemDescantCart(sphericalVector);

  HistoManager::Instance().Descant(true);
}

void DetectorConstruction::AddApparatusDescantStructure() {
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  ApparatusDescantStructure* pApparatusDescantStructure = new ApparatusDescantStructure() ;
  pApparatusDescantStructure->Build() ;
  pApparatusDescantStructure->PlaceDescantStructure( fLogicWorld );

  //pApparatusDescantStructure->PlaceDetector( fLogicWorld, ndet ) ;
}

void DetectorConstruction::AddDetectionSystemTestcan(G4ThreeVector input) {
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  G4double length = G4double(input.x())*cm;
  G4double radius = G4double(input.y())*cm;

  DetectionSystemTestcan* pDetectionSystemTestcan = new DetectionSystemTestcan(length, radius);
  pDetectionSystemTestcan->Build();
  pDetectionSystemTestcan->PlaceDetector(fLogicWorld);

  HistoManager::Instance().Testcan(true);
}

void DetectorConstruction::AddDetectionSystemSpice(G4int nRings) {
  if(fLogicWorld == NULL) {
	Construct();
  }
  DetectionSystemSpice* pSpice = new DetectionSystemSpice() ;
  pSpice->Build(); 
  pSpice->PlaceDetector(fLogicWorld, nRings); //adds the detector rings/segments, als oincludes functions for other builds
  
  HistoManager::Instance().Spice(true);//boolean needed to make SPICE histograms
}

void DetectorConstruction::AddDetectionSystemPaces(G4int ndet) {
  if(fLogicWorld == NULL) {
	 Construct();
  }
  
  DetectionSystemPaces* pPaces = new DetectionSystemPaces() ;
  pPaces->Build() ;

  pPaces->PlaceDetector( fLogicWorld, ndet ) ;

  HistoManager::Instance().Paces(true);
}

void DetectorConstruction::SetSpiceRes(G4bool input){
  HistoManager::Instance().SpiceResolution(input);
}
