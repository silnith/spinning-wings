#include <Windows.h>
#include <GL/glew.h>

#include <array>
#include <memory>
#include <stdexcept>
#include <string>

#include <cassert>

#include "ModelViewProjectionUniformBuffer.h"

namespace silnith::wings::gl4
{

	std::string const ModelViewProjectionUniformBuffer::uniformBlockDeclaration{
		R"shaderText(
uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 projection;
};
)shaderText"
	};

	static GLsizei constexpr numUniforms{ 3 };
	static constexpr GLchar const* uniformBlockName{ "ModelViewProjection" };
	static std::array<GLchar const*, numUniforms> constexpr uniformVariableNames{ "model", "view", "projection" };

	std::shared_ptr<ModelViewProjectionUniformBuffer const> ModelViewProjectionUniformBuffer::MakeBuffer(GLuint programName, GLuint bindingPoint)
	{
		/*
		 * Find the locations (indices) of the components of the uniform block.
		 */
		std::array<GLuint, numUniforms> uniformIndices{ 0, 0, 0 };
		glGetUniformIndices(programName, numUniforms, uniformVariableNames.data(), uniformIndices.data());

		/*
		 * For each index in the uniform block, find the data offset for that index.
		 * This, finally, is the data offset we need to write to in order to set the
		 * data for that component.
		 */
		std::array<GLint, numUniforms> uniformOffsets{ 0, 0, 0, };
		glGetActiveUniformsiv(programName, numUniforms, uniformIndices.data(), GL_UNIFORM_OFFSET, uniformOffsets.data());
		static_assert(uniformVariableNames[0] == "model");
		GLintptr const modelOffset{ uniformOffsets[0] };
		static_assert(uniformVariableNames[1] == "view");
		GLintptr const viewOffset{ uniformOffsets[1] };
		static_assert(uniformVariableNames[2] == "projection");
		GLintptr const projectionOffset{ uniformOffsets[2] };

#if !defined(NDEBUG)
		/*
		 * Confirm that the uniform variables are of the correct type.
		 */
		std::array<GLint, numUniforms> uniformTypes{ 0, 0, 0, };
		glGetActiveUniformsiv(programName, numUniforms, uniformIndices.data(), GL_UNIFORM_TYPE, uniformTypes.data());
		assert(uniformTypes[0] == GL_FLOAT_MAT4);
		assert(uniformTypes[1] == GL_FLOAT_MAT4);
		assert(uniformTypes[2] == GL_FLOAT_MAT4);

		/*
		 * The ModelViewProjectionUniformBuffer class will write the matrices as column-major.
		 */
		std::array<GLint, numUniforms> isRowMajor{ 0, 0, 0, };
		glGetActiveUniformsiv(programName, numUniforms, uniformIndices.data(), GL_UNIFORM_IS_ROW_MAJOR, isRowMajor.data());
		assert(isRowMajor[0] == 0);
		assert(isRowMajor[1] == 0);
		assert(isRowMajor[2] == 0);
#endif

		/*
		 * Get the location (index) of the uniform block.
		 */
		GLuint const blockIndex{ glGetUniformBlockIndex(programName, uniformBlockName) };
		if (blockIndex == GL_INVALID_INDEX)
		{
			throw std::runtime_error{ std::string{ uniformBlockName } + " is not an active uniform block for the program object." };
		}

		/*
		 * Map the local index for the named uniform block to the
		 * global binding point that the buffer is bound to.
		 */
		glUniformBlockBinding(programName, blockIndex, bindingPoint);

		/*
		 * Find the data size required for the uniform block.
		 */
		GLint modelViewProjectionUniformDataSize{ 0 };
		glGetActiveUniformBlockiv(programName, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &modelViewProjectionUniformDataSize);

		/*
		 * Allocate a buffer that can be bound for the uniform block.  Whatever
		 * is written to this buffer will be available in the GLSL program as
		 * the named uniform variables.
		 */
		return std::make_shared<ModelViewProjectionUniformBuffer const>(
			bindingPoint,
			modelViewProjectionUniformDataSize,
			modelOffset, viewOffset, projectionOffset);
	}

	ModelViewProjectionUniformBuffer::ModelViewProjectionUniformBuffer(
		GLuint bindingPoint,
		GLsizei dataSize,
		GLintptr modelOffset,
		GLintptr viewOffset,
		GLintptr projectionOffset)
		: Buffer{},
		bindingPoint{ bindingPoint },
		modelOffset{ modelOffset },
		viewOffset{ viewOffset },
		projectionOffset{ projectionOffset }
	{
		/*
		 * The C++ syntax makes this look like it is row-major, but OpenGL will read it as column-major.
		 * However, that is irrelevant because the identity matrix is its own transposition.
		 */
		std::array<GLfloat, 16> constexpr identity{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,
		};
		GLsizeiptr constexpr identityDataSize{ sizeof(GLfloat) * identity.size() };

		/*
		 * This serves double-duty.  It binds this buffer as the currently
		 * active uniform buffer, so subsequent buffer data will go into this
		 * buffer.  It also binds this buffer to the specified uniform buffer
		 * binding point, so program objects can map their local uniform
		 * indices to the shared binding point.
		 */
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, GetName());
		/*
		 * First we need to allocate storage space for the uniform buffer.
		 */
		glBufferData(GL_UNIFORM_BUFFER, dataSize, nullptr, GL_STATIC_DRAW);
		/*
		 * Then we initialize the components of the uniform block.
		 * Each component is initialized separately.
		 */
		glBufferSubData(GL_UNIFORM_BUFFER, modelOffset, identityDataSize, identity.data());
		glBufferSubData(GL_UNIFORM_BUFFER, viewOffset, identityDataSize, identity.data());
		/*
		 * The projection matrix is initialized here to the identity matrix.
		 * It will be replaced with the real projection matrix later when the window is resized.
		 */
		glBufferSubData(GL_UNIFORM_BUFFER, projectionOffset, identityDataSize, identity.data());
		/*
		 * Note that this only clears the binding of the generic uniform buffer
		 * which is used for providing data.  It does not disturb the global
		 * uniform buffer binding point, so programs that map their local
		 * uniform block index to the global binding point will still work.
		 */
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	ModelViewProjectionUniformBuffer::~ModelViewProjectionUniformBuffer(void) noexcept
	{
		/*
		 * Clear the binding of this buffer in the global array of uniform
		 * buffer binding points.
		 */
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, 0);
	}

	GLuint ModelViewProjectionUniformBuffer::getBindingPoint(void) const noexcept
	{
		return bindingPoint;
	}

	void ModelViewProjectionUniformBuffer::UseForProgram(GLuint programName) const
	{
		/*
		 * Get the location (index) of the uniform block.
		 */
		GLuint const blockIndex{ glGetUniformBlockIndex(programName, uniformBlockName) };
		if (blockIndex == GL_INVALID_INDEX)
		{
			throw std::runtime_error{ std::string{ uniformBlockName } + " is not an active uniform block for the program object." };
		}

		/*
		 * Map the local index for the named uniform block to the
		 * global binding point that the buffer is bound to.
		 */
		glUniformBlockBinding(programName, blockIndex, bindingPoint);
	}

	void ModelViewProjectionUniformBuffer::SetModelMatrix(std::array<GLfloat, 4 * 4> const& modelMatrix) const
	{
		SetMatrix(modelOffset, modelMatrix);
	}

	void ModelViewProjectionUniformBuffer::SetViewMatrix(std::array<GLfloat, 4 * 4> const& viewMatrix) const
	{
		SetMatrix(viewOffset, viewMatrix);
	}

	void ModelViewProjectionUniformBuffer::SetProjectionMatrix(std::array<GLfloat, 4 * 4> const& projectionMatrix) const
	{
		SetMatrix(projectionOffset, projectionMatrix);
	}

	void ModelViewProjectionUniformBuffer::SetMatrix(GLintptr offset, std::array<GLfloat, 4 * 4> const& matrix) const
	{
		GLsizeiptr const dataSize{ static_cast<GLsizeiptr>(sizeof(GLfloat) * matrix.size()) };

		glBindBuffer(GL_UNIFORM_BUFFER, GetName());
		glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, matrix.data());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

}
