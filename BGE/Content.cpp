#include "Content.h"
#include <string>

using namespace BGE;

string Content::prefix = "Content/";
map<string, Model *> Content::models = map<string, Model *>();
map<string, GLuint> Content::textures = map<string, GLuint>();
map<string, GLuint> Content::shaders = map<string, GLuint>();	

Model * Content::LoadModel(string name) {

	// First check to see if it's already loaded and if so, just return it
	map<string, Model *>::iterator mit = Content::models.find(name);
	if (mit != Content::models.end())
	{
		return mit->second;
	}

	std::vector<unsigned int> vertexIndices, normalIndices, uvIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec3> tempColours;
	std::vector<glm::vec3> temp_normals;
	
	map<string, glm::vec3> diffuse;
	bool hasUVs = false;

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

				// Are there any texels? IF so parse them
				if (s.find("//") == string::npos)
				{
					hasUVs = true;
					// Much nicer to do this parse in C
					int matches = sscanf(s.substr(2).c_str()
						,"%d/%d/%d %d/%d/%d %d/%d/%d"
						,&vertexIndex[0]
						,&uvIndex[0]
						,&normalIndex[0]
						,&vertexIndex[1]
						,&uvIndex[1]
						,&normalIndex[1]
						,&vertexIndex[2]
						,&uvIndex[2]
						,&normalIndex[2] 
						);
					if (matches != 9){
						cout << "File can't be read by our simple parser :-( Try exporting with other options\n" << endl;
						return NULL;
					}
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);				
				}
				else
				{
					hasUVs = false;
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
	Content::models[name] = model;
	return model;
}

GLuint Content::LoadTexture(std::string textureName)
{

	// First check to see if it's already loaded and if so, just return it
	map<string, GLuint>::iterator it = Content::textures.find(textureName);
	if (it != Content::textures.end())
	{
		return it->second;
	}

	string path = Content::prefix + textureName + ".bmp";	
	GLuint texture;
	// Now the texture...
	SDL_Surface *surface;	// This surface will tell us the details of the image
	surface = SDL_LoadBMP(path.c_str());
	if (surface == NULL) {
		cout << "Image not loaded" << endl;
		exit(0);
	}
	// Check that the image's width is a power of 2
	if ( (surface->w & (surface->w - 1)) != 0 ) {
		cout << "warning: image.bmp's width is not a power of 2" << endl;
	}
 
	GLint  nOfColors;
	GLenum texture_format;
	// Also check if the height is a power of 2
	if ( (surface->h & (surface->h - 1)) != 0 ) {
		printf("warning: image.bmp's height is not a power of 2\n");
	}
 
	// get the number of channels in the SDL surface
	nOfColors = surface->format->BytesPerPixel;
	if (nOfColors == 4) {
		if (surface->format->Rmask == 0x000000ff) {
			texture_format = GL_RGBA;
		}
		else {
			texture_format = GL_BGRA;
		}
	} else if (nOfColors == 3) {
		if (surface->format->Rmask == 0x000000ff) {
				texture_format = GL_RGB;
		}
		else {
			texture_format = GL_BGR;
		}
	} else {
		printf("warning: the image is not truecolor..  this will probably break\n");
	}

	// Create the texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
 	glTexImage2D(GL_TEXTURE_2D, 0,nOfColors, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);
 
	// Generate mipmaps, by the way.
	glGenerateMipmap(GL_TEXTURE_2D);
	// When MAGnifying the image (no bigger mipmap available), use LINEAR filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	GLfloat maxAniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
	glSamplerParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	textures[textureName] = texture;

	return texture;
}

GLuint Content::LoadShaderPair(string name) {

	// First check to see if it's already loaded and if so, just return it
	map<string, GLuint>::iterator it = Content::shaders.find(name);
	if (it != Content::shaders.end())
	{
		return it->second;
	}
	string vertexFilePath = Content::prefix + name + ".vertexshader";
	string fragmentFilePath = Content::prefix + name + ".fragmentshader";
	
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
 
	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertexFilePath, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
 
	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragmentFilePath, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
 
	GLint Result = GL_FALSE;
	int InfoLogLength;
 
	// Compile Vertex Shader
	cout << "Compiling shader: " << vertexFilePath << endl;
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);
 
	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
 
	// Compile Fragment Shader
	cout << "Compiling shader: " << fragmentFilePath << endl;
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);
 
	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
 
	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);
 
	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
 
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
 
	shaders[name] = ProgramID;
	return ProgramID;
}