


struct IndexBuffer
{
	GLuint bufferID;
	GLuint count;
};

static void 
Initialize(IndexBuffer* indexBuffer, GLuint *data, GLsizei count)
{
	indexBuffer->count = count;
	glGenBuffers(1, &indexBuffer->bufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->bufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*count, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void
DeInitialize(IndexBuffer* indexBuffer, int bp)
{
	glDeleteBuffers(1, &indexBuffer->bufferID);
}

static void
Bind(IndexBuffer* indexBuffer) 
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->bufferID);
}

static void
Unbind(IndexBuffer* indexBuffer) 
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

