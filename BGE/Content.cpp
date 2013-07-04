#include "Content.h"
#include <string>

string BGE::Content::prefix = "Assets/";

BGE::Model * BGE::Content::SimpleLoadModel(string path)
{
	Model * model = new Model();
	cout << path << endl;

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path.c_str(), "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		return false;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		
		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];
		
		// Put the attributes in buffers
		model->vertices.push_back(vertex);
		//out_uvs     .push_back(uv);
		model->normals.push_back(normal);
	
	}
	return model;
}

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