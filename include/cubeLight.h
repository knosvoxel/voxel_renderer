#ifndef CUBELIGHT_H
#define CUBELIGHT_H

#include <glm/glm.hpp>

#include <iostream>
#include <sstream>
#include <string>

const float vertices[]{
    -0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f, 
     0.5f,  0.5f, -0.5f, 
     0.5f,  0.5f, -0.5f, 
    -0.5f,  0.5f, -0.5f,  
    -0.5f, -0.5f, -0.5f, 

    -0.5f, -0.5f,  0.5f, 
     0.5f, -0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f,  0.5f,  
    -0.5f, -0.5f,  0.5f,  

    -0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f, -0.5f, 
    -0.5f, -0.5f, -0.5f, 
    -0.5f, -0.5f, -0.5f, 
    -0.5f, -0.5f,  0.5f, 
    -0.5f,  0.5f,  0.5f, 

     0.5f,  0.5f,  0.5f,  
     0.5f,  0.5f, -0.5f, 
     0.5f, -0.5f, -0.5f, 
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f, 

    -0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f, 
     0.5f, -0.5f,  0.5f, 
     0.5f, -0.5f,  0.5f, 
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,  
     0.5f,  0.5f, -0.5f,  
     0.5f,  0.5f,  0.5f, 
     0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f, -0.5f, 
};

std::vector<glm::vec3>  lightPositions;

class CubeLights{

    public:

        CubeLights(){
            glBindVertexArray(VAO);

            setupMesh();

        }

        void addLight(glm::vec3 position)
        {
            lightPositions.push_back(position);
        }

        void Draw(Shader &shader)
        {
            shader.setInt("lightAmount", lightPositions.size());
            for(unsigned int i = 0; i < lightPositions.size(); i++)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, lightPositions[i]);
                model = glm::scale(model, glm::vec3(0.2f));
                shader.setMat4("model", model);

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        void setLightValues(int position, Shader &shader)
        {
            std::ostringstream stream;
            stream << "pointLights[" << position << "]";

            std::string result = stream.str();

            stream.clear();

            stream << result << ".position";
            std::string shaderPosition = stream.str();
            shader.setVec3(shaderPosition, lightPositions[position]);

            stream.clear();
            
            stream << result << ".ambient";
            std::string shaderAmbient = stream.str();
            shader.setVec3(shaderAmbient, 0.05f, 0.05f, 0.05f);

            stream.clear();

            stream << result << ".diffuse";
            std::string shaderDiffuse = stream.str();
            shader.setVec3(shaderDiffuse, 0.8f, 0.8f, 0.8f);

            stream.clear();

            stream << result << ".specular";
            std::string shaderSpecular = stream.str();
            shader.setVec3(shaderSpecular, 1.0f, 1.0f, 1.0f);

            stream.clear();

            stream << result << ".constant";
            std::string shaderConstant = stream.str();
            shader.setFloat(shaderConstant, 1.0f);

            stream.clear();

            stream << result << ".linear";
            std::string shaderLinear = stream.str();
            shader.setFloat(shaderLinear, 0.09f);

            stream.clear();

            stream << result << ".quadratic";
            std::string shaderQuadratic = stream.str();
            shader.setFloat(shaderQuadratic, 0.032f);
        }
    private:
        unsigned int VBO, VAO;

        void setupMesh()
        {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);

            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            glBindVertexArray(0);
        }
};

#endif