#include "MyOctantExample.h"
using namespace Simplex;

uint MyOctant::m_uOctantCount; //will store the number of octants instantiated
uint MyOctant::m_uMaxLevel;//will store the maximum level an octant can go to
uint MyOctant::m_uIdealEntityCount; //will tell how many ideal Entities this object will contain

/*

USAGE: Constructor, will create an octant containing all MagnaEntities Instances in the Mesh
manager	currently contains
ARGUMENTS:
- uint a_nMaxLevel = 2 -> Sets the maximum level of subdivision
- uint nIdealEntityCount = 5 -> Sets the ideal level of objects per octant
OUTPUT: class object
*/
MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount) {
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uMaxLevel = a_nMaxLevel;
	isRoot = true;	
	m_pRoot = this;
	m_pEntityMngr = MyEntityManager::GetInstance();  //Sets the singleton
	m_pMeshMngr = MeshManager::GetInstance();  //sets the singleton
	m_fSize = 70.0f;
	m_v3Max = vector3(m_fSize / 2);
	m_v3Min = -vector3(m_fSize / 2);
	//Adds every entity currently there
	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++) {
		m_EntityList.push_back(i);
	}
	
}

/*
USAGE: Constructor
ARGUMENTS:
- vector3 a_v3Center -> Center of the octant in global space
- float a_fSize -> size of each side of the octant volume
OUTPUT: class object
*/
MyOctant::MyOctant(vector3 a_v3Center, float a_fSize) {
	m_v3Center = a_v3Center;
	m_fSize = glm::abs(a_fSize);
	m_v3Max = m_v3Center + vector3(m_fSize / 2);
	m_v3Min = m_v3Center - vector3(m_fSize / 2);
	m_pEntityMngr = MyEntityManager::GetInstance();
	m_pMeshMngr = MeshManager::GetInstance();
	isRoot = false;
}

/*
USAGE: Copy Constructor
ARGUMENTS: class object to copy
OUTPUT: class object instance
*/
MyOctant::MyOctant(MyOctant const& other) {
	m_uOctantCount = other.m_uOctantCount; //will store the number of octants instantiated
	m_uMaxLevel = other.m_uMaxLevel;//will store the maximum level an octant can go to
	m_uIdealEntityCount = other.m_uIdealEntityCount; //will tell how many ideal Entities this object will contain

	m_uID = other.m_uID; //Will store the current ID for this octant
	m_uLevel = other.m_uLevel; //Will store the current level of the octant
	m_uChildren = other.m_uChildren;// Number of children on the octant (either 0 or 8)

	m_fSize = other.m_fSize; //Size of the octant

	m_pMeshMngr = other.m_pMeshMngr;//Mesh Manager singleton
	m_pEntityMngr = other.m_pEntityMngr; //Entity Manager Singleton

	m_v3Center = other.m_v3Center; //Will store the center point of the octant
	m_v3Min = other.m_v3Min; //Will store the minimum vector of the octant
	m_v3Max = other.m_v3Max; //Will store the maximum vector of the octant

	m_pParent = other.m_pParent;// Will store the parent of current octant
	for (int i = 0; i < 8; i++) {
		m_pChild[i] = other.m_pChild[i];//Will store the children of the current octant
	}

	m_EntityList = other.m_EntityList; //List of Entities under this octant (Index in Entity Manager)

	m_pRoot = other.m_pRoot;//Root octant
	m_lChild = other.m_lChild; //list of nodes that contain objects (this will be applied to root only)

}
/*
USAGE: Copy Assignment Operator
ARGUMENTS: class object to copy
OUTPUT: ---
*/
MyOctant& MyOctant::operator=(MyOctant const& other) {
	if (this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}

/*
USAGE: Destructor
ARGUMENTS: ---
OUTPUT: ---
*/
MyOctant::~MyOctant(void) {
	Release();
}

/*
USAGE: Changes object contents for other object's
ARGUMENTS:
- MyOctant& other -> object to swap content from
OUTPUT: ---
*/
void MyOctant::Swap(MyOctant& other) {
	std::swap(m_uOctantCount,other.m_uOctantCount); //will store the number of octants instantiated
	std::swap(m_uMaxLevel,other.m_uMaxLevel);//will store the maximum level an octant can go to
	std::swap(m_uIdealEntityCount,other.m_uIdealEntityCount); //will tell how many ideal Entities this object will contain

	std::swap(m_uID,other.m_uID); //Will store the current ID for this octant
	std::swap(m_uLevel,other.m_uLevel); //Will store the current level of the octant
	std::swap(m_uChildren,other.m_uChildren);// Number of children on the octant (either 0 or 8)

	std::swap(m_fSize,other.m_fSize); //Size of the octant

	std::swap(m_pMeshMngr,other.m_pMeshMngr);//Mesh Manager singleton
	std::swap(m_pEntityMngr,other.m_pEntityMngr); //Entity Manager Singleton

	std::swap(m_v3Center,other.m_v3Center); //Will store the center point of the octant
	std::swap(m_v3Min,other.m_v3Min); //Will store the minimum vector of the octant
	std::swap(m_v3Max,other.m_v3Max); //Will store the maximum vector of the octant

	std::swap(m_pParent,other.m_pParent);// Will store the parent of current octant

	for (int i = 0; i < 8; i++) {
		std::swap(m_pChild[i],other.m_pChild[i]);//Will store the children of the current octant
	}

	std::swap(m_EntityList,other.m_EntityList); //List of Entities under this octant (Index in Entity Manager)

	std::swap(m_pRoot,other.m_pRoot);//Root octant
	std::swap(m_lChild,other.m_lChild); //list of nodes that contain objects (this will be applied to root only)
}
/*
USAGE: Gets this octant's size
ARGUMENTS: ---
OUTPUT: size of octant
*/
float MyOctant::GetSize(void) {
	return m_fSize;
}
/*
USAGE: Gets the center of the octant in global scape
ARGUMENTS: ---
OUTPUT: Center of the octant in global space
*/
vector3 MyOctant::GetCenterGlobal(void) {
	return m_v3Center;
}
/*
USAGE: Gets the min corner of the octant in global space
ARGUMENTS: ---
OUTPUT: Minimum in global space
*/
vector3 MyOctant::GetMinGlobal(void) {
	return m_v3Min;
}
/*
USAGE: Gets the max corner of the octant in global space
ARGUMENTS: ---
OUTPUT: Maximum in global space
*/
vector3 MyOctant::GetMaxGlobal(void){
	return m_v3Max;
}

/*
USAGE: Asks if there is a collision with the Entity specified by index from
the Bounding Object Manager
ARGUMENTS:
- int a_uRBIndex -> Index of the Entity in the Entity Manager
OUTPUT: check of the collision
*/
bool MyOctant::IsColliding(uint a_uRBIndex) {
	bool bColliding = true;
	MyRigidBody* thisEnt = m_pEntityMngr->GetEntity(a_uRBIndex)->GetRigidBody();  //Gets the rigid body for collision tests
	
	vector3 minGlobal = thisEnt->GetMinGlobal(); //creates a local min and max
	vector3 maxGlobal = thisEnt->GetMaxGlobal();
	
	//Tests collisions
	if (m_v3Max.x < minGlobal.x) //this to the right of other
		bColliding = false;
	if (m_v3Min.x > maxGlobal.x) //this to the left of other
		bColliding = false;
	
	if (m_v3Max.y < minGlobal.y) //this below of other
		bColliding = false;
	if (m_v3Min.y > maxGlobal.y) //this above of other
		bColliding = false;

	if (m_v3Max.z < minGlobal.z) //this behind of other
		bColliding = false;
	if (m_v3Min.z > maxGlobal.z) //this in front of other
		bColliding = false;
	
	return bColliding;
}
/*
USAGE: Displays the MyOctant volume specified by index including the objects underneath
ARGUMENTS:
- uint a_nIndex -> MyOctant to be displayed.
- vector3 a_v3Color = REYELLOW -> Color of the volume to display.
OUTPUT: ---
*/
void MyOctant::Display(uint a_nIndex, vector3 a_v3Color) {
	matrix4 toWorld = IDENTITY_M4;
	toWorld = glm::translate(toWorld, m_lChild[a_nIndex]->m_v3Center);  //Translates
	vector3 scale = vector3(m_lChild[a_nIndex]->m_fSize, m_lChild[a_nIndex]->m_fSize, m_lChild[a_nIndex]->m_fSize);
	toWorld = glm::scale(toWorld,scale);  //Scales
	m_pMeshMngr->AddWireCubeToRenderList(toWorld, a_v3Color);  //Draws
}
/*
USAGE: Displays the MyOctant volume in the color specified
ARGUMENTS:
- vector3 a_v3Color = REYELLOW -> Color of the volume to display.
OUTPUT: ---
*/
void MyOctant::Display(vector3 a_v3Color) {
	for (int i = 0; i < idList.size(); i++) {
		matrix4 toWorld = IDENTITY_M4;
		toWorld = glm::translate(toWorld, m_v3Center);  //Translates
		vector3 scale = vector3(m_fSize, m_fSize, m_fSize);
		toWorld = glm::scale(toWorld, scale);  //Scales
		m_pMeshMngr->AddWireCubeToRenderList(toWorld, a_v3Color);
	}
}
/*
USAGE: Displays the non empty leafs in the octree
ARGUMENTS:
- vector3 a_v3Color = REYELLOW -> Color of the volume to display.
OUTPUT: ---
*/
void MyOctant::DisplayLeafs(vector3 a_v3Color) {
	for (int i = 0; i < idList.size(); i++) {
		//Tests for leaf
		if (m_lChild[idList[i]]->IsLeaf()) {
			matrix4 toWorld = IDENTITY_M4;
			vector3 scale = vector3(m_lChild[idList[i]]->m_fSize, m_lChild[idList[i]]->m_fSize, m_lChild[idList[i]]->m_fSize);  //Creates scale
			toWorld = glm::translate(toWorld, m_lChild[idList[i]]->m_v3Center);  //Translates
			toWorld = glm::scale(toWorld, scale);  //Scales
			m_pMeshMngr->AddWireCubeToRenderList(toWorld, a_v3Color);  //Draws
		}
	}
}
/*
USAGE: Clears the Entity list for each node
ARGUMENTS: ---
OUTPUT: ---
*/
void MyOctant::ClearEntityList(void) {
	for (int i = 0; i < m_lChild.size(); i++) {
		m_lChild[i]->m_EntityList.clear();
	}
	m_pEntityMngr->ClearDimensionSetAll();
}
/*
USAGE: allocates 8 smaller octants in the child pointers
ARGUMENTS: ---
OUTPUT: ---
*/
void MyOctant::Subdivide(void) {
	//Makes sure this is fine
	if (this->m_uLevel < this->m_uMaxLevel && this->m_EntityList.size() > this->m_uIdealEntityCount) {
		this->m_uChildren = 8;  //Sets children number
		vector3 corner1 = vector3(m_v3Center.x + m_v3Min.x / 2, m_v3Center.y + m_v3Min.y / 2, m_v3Center.z + m_v3Min.z / 2);  //Creates new center
		float dist = m_v3Center.x - m_v3Min.x;  //Creates new size
		m_pChild[0] = new MyOctant(corner1, dist);  //Creates new Octant

		//Rinse and Repeat
		corner1 = vector3(m_v3Center.x + m_v3Max.x / 2, m_v3Center.y + m_v3Min.y / 2, m_v3Center.z + m_v3Min.z / 2);
		m_pChild[1] = new MyOctant(corner1, dist);
		corner1 = vector3(m_v3Center.x + m_v3Min.x / 2, m_v3Center.y + m_v3Max.y / 2, m_v3Center.z + m_v3Min.z / 2);
		m_pChild[2] = new MyOctant(corner1, dist);
		corner1 = vector3(m_v3Center.x + m_v3Max.x / 2, m_v3Center.y + m_v3Max.y / 2, m_v3Center.z + m_v3Min.z / 2);
		m_pChild[3] = new MyOctant(corner1, dist);
		corner1 = vector3(m_v3Center.x + m_v3Min.x / 2, m_v3Center.y + m_v3Min.y / 2, m_v3Center.z + m_v3Max.z / 2);
		m_pChild[4] = new MyOctant(corner1, dist);
		corner1 = vector3(m_v3Center.x + m_v3Max.x / 2, m_v3Center.y + m_v3Min.y / 2, m_v3Center.z + m_v3Max.z / 2);
		m_pChild[5] = new MyOctant(corner1, dist);
		corner1 = vector3(m_v3Center.x + m_v3Min.x / 2, m_v3Center.y + m_v3Max.y / 2, m_v3Center.z + m_v3Max.z / 2);
		m_pChild[6] = new MyOctant(corner1, dist);
		corner1 = vector3(m_v3Center.x + m_v3Max.x / 2, m_v3Center.y + m_v3Max.y / 2, m_v3Center.z + m_v3Max.z / 2);
		m_pChild[7] = new MyOctant(corner1, dist);
		
		uint level = this->m_uLevel+1;  //Ups the level of the children
		for (int i = 0; i < 8; i++) {
			m_pChild[i]->m_uLevel = level;  //Sets level
			m_pChild[i]->m_pParent = this;  //Sets parent
			//Tests for entities contained within
			for (int j = 0; j < m_EntityList.size(); j++) {
				if (m_pChild[i]->IsColliding(m_EntityList[j]))
				{
					m_pChild[i]->m_EntityList.push_back(m_EntityList[j]);
				}
			}
			//Sets root accordingly
			if (isRoot == false) {
				m_pChild[i]->m_pRoot = m_pRoot;
			}
			else {
				m_pChild[i]->m_pRoot = this;
			}
			//if (m_pChild[i]->m_EntityList.size() > 0) {  breaks under certain circustances
			m_uOctantCount++;
			m_pChild[i]->m_uID = m_uOctantCount;  //sets id
			//Recursion!
			if (m_pChild[i]->m_uLevel < m_pChild[i]->m_uMaxLevel && m_pChild[i]->m_EntityList.size()>m_pChild[i]->m_uIdealEntityCount) {
				m_pChild[i]->Subdivide();
			}
			//if(m_pChild[i]->IsLeaf()
			m_pRoot->m_lChild.push_back(m_pChild[i]);  //Adds to the big boy pile (the final ones)
		//}
		}
	}
}
/*
USAGE: returns the child specified in the index
ARGUMENTS: uint a_nChild -> index of the child (from 0 to 7)
OUTPUT: MyOctant object (child in index)
*/
MyOctant* MyOctant::GetChild(uint a_nChild) {
	return m_pChild[a_nChild];
}
/*
USAGE: returns the parent of the octant
ARGUMENTS: ---
OUTPUT: MyOctant object (parent)
*/
MyOctant* MyOctant::GetParent(void) {
	return m_pParent;
}
/*
USAGE: Asks the MyOctant if it does not contain any children (its a leaf)
ARGUMENTS: ---
OUTPUT: It contains no children
*/
bool MyOctant::IsLeaf(void) {
	if (m_uChildren == 0) {
		return true;
	}
	return false;
}
/*
USAGE: Asks the MyOctant if it contains more than this many Bounding Objects
ARGUMENTS:
- uint a_nEntities -> Number of Entities to query
OUTPUT: It contains at least this many Entities
*/
bool MyOctant::ContainsMoreThan(uint a_nEntities) {
	if (m_EntityList.size() > a_nEntities) {
		return true;
	}
	return false;
}
/*
USAGE: Deletes all children and the children of their children (almost sounds apocalyptic)
ARGUMENTS: ---
OUTPUT: ---
*/
void MyOctant::KillBranches(void) {
	//for (int i = m_lChild.size() - 1; i >= 0; i--) {
	//	SafeDelete(m_lChild[i]);
	//}
	m_lChild.clear();
	m_uOctantCount = 0;
	m_uChildren = 0;
	idList.clear();
}
/*
USAGE: Creates a tree using subdivisions, the max number of objects and levels
ARGUMENTS:
- uint a_nMaxLevel = 3 -> Sets the maximum level of the tree while constructing it
OUTPUT: ---
*/
void MyOctant::ConstructTree(uint a_nMaxLevel) {

	Subdivide();
	ConstructList();
	//Subdivide();
}
/*
USAGE: Traverse the tree up to the leafs and sets the objects in them to the index
ARGUMENTS: ---
OUTPUT: ---
*/
void MyOctant::AssignIDtoEntity(void) {
	for (int i = 0; i < idList.size(); i++) {
		for (int j = 0; j < m_lChild[idList[i]]->m_EntityList.size(); j++) {
			m_pEntityMngr->AddDimension(m_lChild[idList[i]]->m_EntityList[j],idList[i]);
		}
	}
}

/*
USAGE: Gets the total number of octants in the world
ARGUMENTS: ---
OUTPUT: ---
*/
Simplex::uint MyOctant::GetOctantCount(void) {
	Simplex::uint gonow = m_uOctantCount;
	return gonow;
}

	/*
	USAGE: Deallocates member fields
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void MyOctant::Release(void) {
		SafeDelete(m_pParent);
		
		for (int i = 7; i <= 0; i--) {
			MyOctant* pOctant = m_pChild[i];
			SafeDelete(pOctant);
		}
		m_EntityList.clear();
		m_pRoot = nullptr;
		m_lChild.clear();
		idList.clear();
	}
	/*
	USAGE: Allocates member fields
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void MyOctant::Init(void) {
		
	}
	/*
	USAGE: creates the list of all leafs that contains objects.
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void MyOctant::ConstructList(void) {
		for (int i = 0; i < m_pRoot->m_lChild.size(); i++) {
			if (m_lChild[i]->m_EntityList.empty() == false && m_lChild[i]->m_uLevel == m_uMaxLevel) {
				idList.push_back(m_lChild[i]->m_uID);
			}
		}
	}
