#ifndef VOX_LOADING_H
#define VOX_LOADING_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ogt_vox.h"
#include "ogt_voxel_meshify.h"

namespace VoxLoad
{
    enum vertex_input_slot
    {
        k_vertex_input_slot_position = 0, // which input the vertex shader receives position from (eg. "layout(location=0)  in vec3 i_position" )
        k_vertex_input_slot_normal = 1,   // which input the vertex shader receives normal from (eg. "layout(location=1)  in vec3 i_normal" )
        k_vertex_input_slot_color = 2,    // which input the vertex shader receives color from (eg. "layout(location=2)  in vec4 i_color" )
    };

    struct buffer
    {
        GLuint handle;
    };

    bool create_static_buffer(buffer *buffer, uint32_t size, const void *initial_data)
    {
        glGenBuffers(1, &buffer->handle);
        if (!buffer->handle)
            return false;

        glBindBuffer(GL_COPY_WRITE_BUFFER, buffer->handle);
        glBufferData(GL_COPY_WRITE_BUFFER, size, initial_data, GL_STATIC_DRAW);
        return true;
    }

    void destroy_buffer(buffer *buffer)
    {
        glDeleteBuffers(1, &buffer->handle);
        buffer->handle = 0;
    }

    void bind_vertex_buffer(const buffer *buffer)
    {
        glBindBuffer(GL_ARRAY_BUFFER, buffer ? buffer->handle : 0);
        if (buffer)
        {
            glVertexAttribPointer(k_vertex_input_slot_position, 3, GL_FLOAT, GL_FALSE, sizeof(ogt_mesh_vertex), (GLvoid *)(offsetof(ogt_mesh_vertex, pos)));
            glEnableVertexAttribArray(k_vertex_input_slot_position);
            glVertexAttribPointer(k_vertex_input_slot_normal, 3, GL_FLOAT, GL_FALSE, sizeof(ogt_mesh_vertex), (GLvoid *)(offsetof(ogt_mesh_vertex, normal)));
            glEnableVertexAttribArray(k_vertex_input_slot_normal);
            glVertexAttribPointer(k_vertex_input_slot_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ogt_mesh_vertex), (GLvoid *)(offsetof(ogt_mesh_vertex, color)));
            glEnableVertexAttribArray(k_vertex_input_slot_color);
        }
        else
        {
            glDisableVertexAttribArray(k_vertex_input_slot_position);
            glDisableVertexAttribArray(k_vertex_input_slot_normal);
            glDisableVertexAttribArray(k_vertex_input_slot_color);
        }
    }

    void bind_index_buffer(const buffer *buffer)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->handle);
    }

    // transfers data in cpu memory to a given offset of a gpu buffer
    void upload_to_buffer(const buffer *buffer, uint32_t buffer_offset, const void *data, uint32_t data_size)
    {
        glBindBuffer(GL_COPY_WRITE_BUFFER, buffer->handle);
        glBufferSubData(GL_COPY_WRITE_BUFFER, buffer_offset, data_size, data);
    }
}

#endif