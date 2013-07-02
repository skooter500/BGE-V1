#include "Content.h"
#include <string>

string BGE::Content::prefix = "Assets/";

BGE::Model * BGE::Content::LoadModel(string name)
{
	std::vector<unsigned int> vertexIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec3> tempColours;
	std::vector<glm::vec3> temp_normals;
	
	map<string, glm::vec3> diffuse;

	// First load the materials
	string materialsFileName = Content::prefix + name + ".mtl";
	cout << "Loading materials file: " << materialsFileName << endl;
	std::ifstream is;
	
	is.open(materialsFileName, ios::in);	
	if (is.is_open())
	{
		string materialPrefix = "newmtl";
		string diffusePrefix = "Kd";
		string s;	
		string junk;
		string materialName;
		while(!is.eof())
		{
			getline(is,s);
			
			if (s.find(materialPrefix) == 0)
			{
				materialName = s.substr(materialPrefix.length() + 1);
			}
			if (s.find(diffusePrefix) == 0)
			{
				glm::vec3 colour;
				stringstream ss(s);
				ss >> junk >> colour.r >> colour.g >> colour.b;
				diffuse[materialName] = colour;
			}
		}
		is.close();
	}

	string objFileName = Content::prefix + name + ".obj";	
	cout << "Loading OBJ file: " << name << endl;

	is.open(objFileName, ios::in);

	if (is.is_open())
	{
		string s;
		string junk;
		string materialName;
		string materialPrefix = "usemtl";
		while(!is.eof())
		{
			getline(is,s);
			if (s.find("vn ") == 0)
			{
				stringstream ss(s);
				glm::vec3 vertex;
				ss >> junk >> vertex.x >> vertex.y >> vertex.z;
				temp_normals.push_back(vertex);
			}
			else if (s.find("v ") == 0)
			{
				stringstream ss(s);
				glm::vec3 vertex;
				ss >> junk >> vertex.x >> vertex.y >> vertex.z;
				temp_vertices.push_back(vertex);
			}			
			else if (s.find(materialPrefix) == 0)
			{
				materialName = s.substr(materialPrefix.length() + 1);
			}
			else if (s.find("f ") == 0) {
				// Its a face in other words 3 vertices 
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				// Much nicer to do this parse in C
				int matches = sscanf(s.substr(2).c_str()
					,"%d//%d %d//%d %d//%d"
					,&vertexIndex[0]
					,&normalIndex[0]
					,&vertexIndex[1]
					,&normalIndex[1]
					,&vertexIndex[2]
					,&normalIndex[2] 
					);
				if (matches != 6){
					cout << "File can't be read by our simple parser :-( Try exporting with other options\n" << endl;
					return NULL;
				}

				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				
				tempColours.push_back(diffuse[materialName]);
				tempColours.push_back(diffuse[materialName]);
				tempColours.push_back(diffuse[materialName]);

				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
		}
		is.close();
	}
	Model * model = new Model();
	// For each vertex of each triangle
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){
		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int normalIndex = normalIndices[i];
		
		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];
		
		// Put the attributes in buffers
		model->vertices.push_back(vertex);
		model->normals.push_back(normal);
		model->colours.push_back(tempColours[i]);
	
	}
	return model;
}