//   $tigre.2@sympatico.ca, louis.archambault@phy.ulaval.ca
//   03/10/02

//*******************************************************
//
// DicomHandler.hh :
//	- Handling of DICM images
//	- Transforming *.dcm to *.g4 ( pixels->density )
//	- Reading headers and pixels
//	- Transforming pixel to density and creating *.g4
//	  files
//	- Functions are in DicomHandler.cc
//
// The code was written by :
//	Vincent Hubert-Tremblay at tigre.2@sympatico.ca
//
// Base on previous code by :
//	Dragan Tubic <tdragan@gel.ulaval.ca>
//
// For more information contact :
//	Louis Archambault louis.archambault@phy.ulaval.ca
// or
//	Luc Beaulieu beaulieu@phy.ulaval.ca
//
// Centre Hospitalier Universitaire de Quebec (CHUQ),
// Hotel-Dieu de Quebec, departement de Radio-oncologie
// 11 cote du palais. Quebec, QC, Canada, G1R 2J6
// tel (418) 525-4444 #6720
// fax (418) 691 5268
//*******************************************************

#ifndef DicomHandler_h
#define DicomHandler_h 1
#include "globals.hh"

class dicomHandler
{
public:

  dicomHandler()
  {
    compression=0;
    max=0;
  };

  G4int readHeader(FILE *,char[300]);

  G4int readData(FILE *,char[300]); // note: always use readHeader 
                                    // before readData

  // use ImageMagick to display the image
  G4int displayImage(char[500]);
  void checkFileFormat();

private:
  G4int compression, max;
  char name[300], name_in_file[300];

  // Variables used for reading the DICOM images
  unsigned short int read_group_id;
  unsigned short int read_element_id;
  unsigned short int element_length;
  unsigned short int element_length2;
  G4int element_length3;
  char value[10000][300];
  char buffer[196];
  unsigned int Int_Buffer[1000000];
  unsigned int tab[1000][1000];

  // Transform a pixel value to a density
  G4double pixel2density(G4int pixel);

  FILE* data;
  FILE* fdensity;

  G4int tag_dictionnary;
  G4int rows;
  G4int columns;
  G4int bits_stored;
  G4int bits_allocated;
  G4int high_bits;
  G4int len;
  char pixel_spacing[300],pixel_spacing_X[300],pixel_spacing_Y[300];
  char slice_tickness[300];
  G4int rescale_slope;
  G4int rescale_intercept;
  G4double slice_location;

};
#endif

