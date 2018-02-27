#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Michael Campito - mjc1264@g.rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits
	float fRadius = 0.95f; //initial radius of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	int index = 0; //Creates the index for stop point creation
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		indexes.push_back(0); //sets the index value for this new orbit to zero
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		stopList.push_back(std::vector<vector3>()); //Adds a new vector3 for containing positions

		//Loop to calculate stop points
		for (int j = 0; j < i; j++) {
			float angle = ((PI * 2) / i)*j; //Angle for the stop point calculation
			stopList[index].push_back(vector3(fRadius * cos(angle), fRadius * sin(angle), 0)); //Adds a new stop point on that angle
		}
		index++;//Increases the index for the orbit list
		fSize += 0.5f; //increment the size for the next orbit
		fRadius += 0.5f; //Increment the radius for the next orbit as well
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system//Get a timer
	static float fTimer = 0;	//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

	vector3 v3CurrentPos = vector3(0, 0, 0); //creates the current position vector


	
	//-------------------
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 90.0f, AXIS_X));

		//if indexes is at the end of the list
		if (indexes[i] == stopList[i].size() - 1) {
			v3CurrentPos = (1 - fTimer)*stopList[i][indexes[i]] + fTimer * stopList[i][0]; //calculate the current position
		}
		//if indexes is not at the end of the list
		else {
			v3CurrentPos = (1 - fTimer)*stopList[i][indexes[i]] + fTimer * stopList[i][indexes[i] + 1];
		}
		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}
	//If one second has passed
	if (fTimer >= 1) {
		//For each value of stoplist
		for (int i = 0; i < stopList.size(); i++) {
			//Resets an index to zero if at the end
			if (indexes[i] == stopList[i].size() - 1) {
				indexes[i] = 0;
			}
			//Otherwise, increments the index
			else {
				indexes[i]++;
			}
		}
		fTimer = 0; //Resets the timer to zero for a new cycle
	}


	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}