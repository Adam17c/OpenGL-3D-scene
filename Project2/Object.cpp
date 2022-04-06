#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

class Object
{
private:
	int data_byets;
	int vertex_count;
	float *render_data;

	unsigned int textures[2];
	unsigned int *tex[2];
	
public:
	const Object()
	{
		vertex_count = 0;
	}
 
	void LoadObjectMesh(string filename)
	{
		string line;
		string x, y, z;
		string u, v;
		string xn, yn, zn;
		string line_pos_1vert, line_pos_2vert, line_pos_3vert;
		string line_uv_1vert, line_uv_2vert, line_uv_3vert;
		string line_nomrmal_1vert, line_normal_2vert, line_normal_3vert;
		string useless;
		ifstream file;
		int first_line_pos_num_vert = 3;
		int to_render = 0;

		int pos_line_count = 0;
		int uv_line_count = 0;
		int normal_line_count = 0;
		bool verives = true;
		bool uv_s = true;
		bool normal = true;
		bool triangles = true;

		file.open(filename);
		if (file.good() == false)
		{
			cout << "Cannot open the file!";
		}

		for (int index = 0; index < first_line_pos_num_vert; index++)
		{
			getline(file, line);
		}

		while (verives)
		{
			getline(file, line);
			if (line.find("vt") != string::npos)
			{									 
				verives = false;
				uv_line_count++;
			}
			else
			{
				pos_line_count++;
			}
		}
		while (uv_s)
		{
			getline(file, line);
			if (line.find("vn") != string::npos)
			{
				uv_s = false;
				normal_line_count++;
			}
			else
			{
				uv_line_count++;
			}
		}
		while (normal)
		{
			getline(file, line);
			if (line.find("s off") != string::npos)
			{
				normal = false;
			}
			else
			{
				normal_line_count++;
			}
		}

		while (triangles)
		{
			getline(file, line);
			if (line.find("f") != string::npos)
			{
				to_render += 24;
				vertex_count += 3;
			}
			else
			{
				triangles = false;
			}
		}

		render_data = new float[to_render];
		int *ptr_pos_line_num = new int[vertex_count];
		int *ptr_uv_line_num = new int[vertex_count];
		int *ptr_norm_line_num = new int[vertex_count];
		string *ptr_v_string = new string[pos_line_count];
		string *ptr_uv_string = new string[uv_line_count];
		string *ptr_vn_string = new string[normal_line_count];

		file.clear();
		file.seekg(0);

		for (int index = 0; index < first_line_pos_num_vert; index++)
		{
			getline(file, line);
		}

		for (int index = 0; index < pos_line_count; index++)
		{
			getline(file, ptr_v_string[index]);
		}

		for (int index = 0; index < uv_line_count; index++)
		{
			getline(file, ptr_uv_string[index]);
		}

		for (int index = 0; index < normal_line_count; index++)
		{
			getline(file, ptr_vn_string[index]);
		}

		getline(file, line);
		for (int index = 0; index < vertex_count; index += 3)
		{
			getline(file, line);
			stringstream triangle_string(line);
			getline(triangle_string, useless, ' ');
			getline(triangle_string, line_pos_1vert, '/');
			getline(triangle_string, line_uv_1vert, '/');
			getline(triangle_string, line_nomrmal_1vert, ' ');
			getline(triangle_string, line_pos_2vert, '/');
			getline(triangle_string, line_uv_2vert, '/');
			getline(triangle_string, line_normal_2vert, ' ');
			getline(triangle_string, line_pos_3vert, '/');
			getline(triangle_string, line_uv_3vert, '/');
			getline(triangle_string, line_normal_3vert, '\n');

			ptr_pos_line_num[index] = stoi(line_pos_1vert) - 1;
			ptr_uv_line_num[index] = stoi(line_uv_1vert) - 1;
			ptr_norm_line_num[index] = stoi(line_nomrmal_1vert) - 1;

			ptr_pos_line_num[index + 1] = stoi(line_pos_2vert) - 1;
			ptr_uv_line_num[index + 1] = stoi(line_uv_2vert) - 1;
			ptr_norm_line_num[index + 1] = stoi(line_normal_2vert) - 1;

			ptr_pos_line_num[index + 2] = stoi(line_pos_3vert) - 1;
			ptr_uv_line_num[index + 2] = stoi(line_uv_3vert) - 1;
			ptr_norm_line_num[index + 2] = stoi(line_normal_3vert) - 1;
		}

		for (int index = 0; index < vertex_count; index++)
		{
			stringstream v_string(ptr_v_string[ptr_pos_line_num[index]]);
			v_string >> useless >> x >> y >> z;
			render_data[(index * 8)] = stof(x);
			render_data[(index * 8) + 1] = stof(y);
			render_data[(index * 8) + 2] = stof(z);

			stringstream uv_string(ptr_uv_string[ptr_uv_line_num[index]]);
			uv_string >> useless >> u >> v;
			render_data[(index * 8) + 3] = stof(u);
			render_data[(index * 8) + 4] = stof(v);

			stringstream vn_string(ptr_vn_string[ptr_norm_line_num[index]]);
			vn_string >> useless >> xn >> yn >> zn;
			render_data[(index * 8) + 5] = stof(xn);
			render_data[(index * 8) + 6] = stof(yn);
			render_data[(index * 8) + 7] = stof(zn);
		}
		delete[] ptr_pos_line_num;
		delete[] ptr_uv_line_num;
		delete[] ptr_norm_line_num;
		delete[] ptr_v_string;
		delete[] ptr_uv_string;
		delete[] ptr_vn_string;

		file.seekg(0);
		file.close();

		data_byets = to_render * 4;
	}

	void LoadTexture(const char *fname, int id)
	{
		glGenTextures(1, &textures[id]);
		tex[id] = &textures[id];

		glBindTexture(GL_TEXTURE_2D, *tex[id]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width = 0, height = 0, nrChannels = 0;
		unsigned char *data = stbi_load(fname, &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}

	void DrawMesh(int id)
	{	
		int draw_vertices = data_byets / 32;
		glBufferData(GL_ARRAY_BUFFER, data_byets, render_data, GL_STATIC_DRAW);
		glBindTexture(GL_TEXTURE_2D, *tex[id]);
		glDrawArrays(GL_TRIANGLES, 0, draw_vertices);
	}
};

