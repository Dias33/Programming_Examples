/*
	HAPI Start
	----------
	This solution contains an already set up HAPI project and this main file
	
	The directory structure and main files are:

	HAPI_Start - contains the Visual Studio solution file (.sln)
		HAPI_APP - contains the Visual Studio HAPI_APP project file (.vcxproj) and source code
			HAPI - the directory with all the HAPI library files
*/

//  Include the HAPI header to get access to all of HAPIs interfaces

#include <HAPI_lib.h>
#include "World.h"




// HAPI itself is wrapped in the HAPISPACE namespace

using namespace HAPISPACE;

// Every HAPI program has a HAPI_Main as an entry point
// When this function exits the program will close down

void HAPI_Main()
{
	HAPI.SetShowFPS(true);

	CWorld My_World;

	My_World.Initialise(1200,800);

	My_World.LoadLevel();

	while (HAPI.Update())
	{
		My_World.Run();
	}

}



