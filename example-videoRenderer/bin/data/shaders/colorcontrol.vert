void main(void)
{
	//this is a default vertex shader all it does is this...
	gl_Position = ftransform();
	//.. and passes the multi texture coordinates along to the fragment shader
	gl_TexCoord[0] = gl_MultiTexCoord0;
}